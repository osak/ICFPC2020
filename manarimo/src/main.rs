use actix_files as fs;
use actix_web::{web, App, HttpRequest, HttpResponse, HttpServer, get, post, dev::ServiceRequest, Result, error};
use serde::{Serialize, Deserialize};
use chrono::{DateTime, Utc, TimeZone};

use rusoto_core::{Region, RusotoError};
use rusoto_batch::{Batch, BatchClient, ListJobsRequest, DescribeJobsRequest, DescribeJobsError};

#[derive(Serialize)]
struct JobSummary {
    job_id: String,
    job_name: String
}

#[derive(Deserialize)]
struct DescribeRequest {
    job_ids: String
}

#[derive(Serialize)]
struct JobDetail {
    index: Option<u32>,
    job_name: String,
    status: String,
    created_at: DateTime<Utc>,
    started_at: DateTime<Utc>,
    stopped_at: DateTime<Utc>,
}

impl From<rusoto_batch::JobDetail> for JobDetail {
    fn from(obj: rusoto_batch::JobDetail) -> JobDetail {
        JobDetail {
            index: obj.array_properties.map(|p| p.index).flatten().map(|v| v as u32),
            job_name: obj.job_name,
            status: obj.status,
            created_at: Utc.timestamp(obj.created_at.unwrap() / 1000, 0),
            started_at: Utc.timestamp(obj.started_at.unwrap() / 1000, 0),
            stopped_at: Utc.timestamp(obj.stopped_at.unwrap() / 1000, 0),
        }
    }
}

#[get("/list_jobs")]
async fn list_jobs(batch_client: web::Data<BatchClient>) -> Result<web::Json<Vec<JobSummary>>> {
    let mut request = ListJobsRequest::default();
    request.job_queue = Some("arn:aws:batch:ap-northeast-1:806196602578:job-queue/first-run-job-queue".to_owned());
    request.job_status = Some("SUCCEEDED".to_owned());

    match batch_client.list_jobs(request).await {
        Ok(output) => Ok(web::Json(output.job_summary_list.into_iter().map(|js| JobSummary {
            job_id: js.job_id,
            job_name: js.job_name,
        }).collect())),
        Err(e) => Err(error::ErrorInternalServerError(e))
    }
}

async fn describe_array_jobs(batch_client: &BatchClient, base_job_id: String, nodes: i64) -> Result<Vec<JobDetail>, RusotoError<DescribeJobsError>> {
    let mut idx = 0;
    let mut result = vec![];

    loop {
        if idx >= nodes {
            break;
        }
        let job_ids = (idx..idx+100).into_iter()
            .map(|i| format!("{}:{}", base_job_id, i))
            .collect();
        let request = DescribeJobsRequest {
            jobs: job_ids
        };
        let response = batch_client.describe_jobs(request).await?;
        let mut job_details = response.jobs.unwrap()
            .into_iter()
            .map(|j| j.into())
            .collect::<Vec<JobDetail>>();
        result.append(&mut job_details);

        idx += 100;
    }
    Ok(result)
}

#[get("/describe_jobs")]
async fn describe_jobs(batch_client: web::Data<BatchClient>, q: web::Query<DescribeRequest>) -> Result<web::Json<Vec<JobDetail>>> {
    let ids = q.job_ids.split(",").into_iter().map(|s| s.to_owned()).collect();
    let request = DescribeJobsRequest {
        jobs: ids
    };
    let raw_jobs = match batch_client.describe_jobs(request).await {
        Ok(output) => Ok(output.jobs.unwrap()),
        Err(e) => Err(error::ErrorInternalServerError(e))
    }?;

    let mut result = vec![];
    for job in raw_jobs {
        match job.array_properties {
            Some(p) => if let Ok(mut v) = describe_array_jobs(&*batch_client, job.job_id, p.size.unwrap()).await {
                result.append(&mut v)
            },
            None => result.push(job.into())
        }
    }
    Ok(web::Json(result))
}

async fn default_route(req: HttpRequest) -> Result<HttpResponse, std::io::Error> {
    use actix_web::http::header::{HeaderName, HeaderValue};

    match req.path() {
        "/bundle.js" => fs::NamedFile::open("web-dist/bundle.js").map(|n| n.into_response(&req).unwrap()),
        "/style.css" => fs::NamedFile::open("web-dist/style.css").map(|n| n.into_response(&req).unwrap()),
        _ => fs::NamedFile::open("web-dist/index.html")
              .map(|n| {
                  let mut resp = n.into_response(&req).unwrap();
                  let headers = resp.headers_mut();
                  headers.append(HeaderName::from_static("cache-control"), HeaderValue::from_static("no-cache"));
                  resp
              })
    }
}

#[actix_rt::main]
async fn main() -> std::io::Result<()> {
    let port: u32 = std::env::var("PORT").unwrap().parse().unwrap();
    let batch_client = BatchClient::new(Region::ApNortheast1);

    HttpServer::new(move || {
        App::new()
            .data(batch_client.clone())
            .service(web::scope("/api")
                .service(list_jobs)
                .service(describe_jobs))
            .default_service(web::resource("").route(web::get().to(default_route)))
    })
    .bind(format!("0.0.0.0:{}", port))?
    .run()
    .await
}