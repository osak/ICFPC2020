const path = require("path");
const HtmlWebpackPlugin = require("html-webpack-plugin");
const CopyPlugin = require("copy-webpack-plugin");
const webpack = require("webpack");

let constants;
if (process.env.NODE_ENV == "production") {
  constants = {
    API_BASE: JSON.stringify("https://manarimo.herokuapp.com/api"),
    OFFICIAL_API_BASE: JSON.stringify("https://icfpc2020-api.testkontur.ru"),
    OFFICIAL_API_KEY: JSON.stringify("decffdda9f2d431792a37fbfb770f825"),
  };
} else {
  constants = {
    API_BASE: JSON.stringify("http://0.0.0.0:8000/api"),
    OFFICIAL_API_BASE: JSON.stringify("https://icfpc2020-api.testkontur.ru"),
    OFFICIAL_API_KEY: JSON.stringify("decffdda9f2d431792a37fbfb770f825"),
  };
}

module.exports = {
  entry: "./web/index.tsx",
  devtool: "source-map",
  devServer: {
    host: "0.0.0.0",
    historyApiFallback: true,
  },

  module: {
    rules: [
      {
        test: /\.tsx?/,
        use: "ts-loader",
        exclude: /node_modules/,
      },
      {
        test: /\.css$/i,
        use: ["style-loader", "css-loader"],
      },
    ],
  },
  resolve: {
    extensions: [".tsx", ".ts", ".js"],
  },
  output: {
    filename: "bundle.js",
    path: path.resolve(__dirname, "web-dist"),
    publicPath: "/",
  },
  plugins: [
    new HtmlWebpackPlugin({
      template: "./web/index.html",
      hash: true,
    }),
    new webpack.DefinePlugin(constants),
    new CopyPlugin({
      patterns: [{ from: "./web/style.css", to: "style.css" }],
    }),
    new webpack.IgnorePlugin(/^\.\/locale$/, /moment$/),
  ],
};
