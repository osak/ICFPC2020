module.exports = function (req, res, next) {
  if (req.method === "POST") {
    req.method = "GET"; // GETに偽装
  }
  next();
};
