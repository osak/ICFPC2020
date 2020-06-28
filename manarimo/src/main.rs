use actix_files as fs;
use actix_web::{web, App, HttpRequest, HttpResponse, HttpServer, get, post, Error, dev::ServiceRequest};

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
    HttpServer::new(move || {
        App::new()
            .default_service(web::resource("").route(web::get().to(default_route)))
    })
    .bind(format!("0.0.0.0:{}", 8000))?
    .run()
    .await
}