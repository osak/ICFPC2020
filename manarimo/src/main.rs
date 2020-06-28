use actix_files as fs;
use actix_web::{web, App, HttpRequest, HttpResponse, HttpServer, get, post, dev::ServiceRequest, Result, error};
use serde::Serialize;

use rusoto_core::Region;
use rusoto_batch::{Batch, BatchClient, ListJobsRequest};

#[derive(Serialize)]
struct JobSummary {
    job_id: String,
    job_name: String
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
    let batch_client = BatchClient::new(Region::ApNortheast1);

    HttpServer::new(move || {
        App::new()
            .app_data(web::Data::new(batch_client.clone()))
            .service(web::scope("/api")
                .service(list_jobs))
            .default_service(web::resource("").route(web::get().to(default_route)))
    })
    .bind(format!("0.0.0.0:{}", 8000))?
    .run()
    .await
}