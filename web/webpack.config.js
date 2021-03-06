const HtmlWebPackPlugin = require("html-webpack-plugin");
const path = require('path');


//let targetDir = path.resolve(__dirname, '../')
let publicPath = '/numsrv/'


module.exports = {

  output: {
    filename: '[name].js',
    //path: targetDir,
    publicPath: publicPath,
  },


  entry: './src/index.jsx',


  module: {
    rules: [
      {
        test: /\.(js|jsx)$/,
        exclude: /(node_modules)/,
        resolve: {
          extensions: ['.js', '.jsx']
        },
        use: {
          loader: 'babel-loader',
          options: {
            presets: [
              [
                "@babel/preset-env",
                {
                  "targets": {
                    "chrome": "70"
                  }
                }
              ],
              "@babel/preset-react"
            ]
          }
        }
      },
      {
        test: /\.html$/,
        use: [
          {
            loader: "html-loader"
          }
        ]
      },
      {
        test: /\.css$/,
        use: [
          {
             loader: 'style-loader',
          },
          {
            loader: 'css-loader',
          }
        ],
      },
      { test: /\.(png|jpg|gif|ttf|eot|svg)(\?v=[0-9]\.[0-9]\.[0-9])?$/, loader: "file-loader" },
    ]
  },


  plugins: [
    new HtmlWebPackPlugin({
      template: "./src/index.html",
      filename: "./index.html"
    }),
  ],



  externals: {
    'react': 'React',
    'react-dom': 'ReactDOM',
    'react-router': 'ReactRouter',
    'react-bootstrap': 'ReactBootstrap',
  },



  devServer: {
    historyApiFallback: {
      index: publicPath,
    },
    proxy: {
      '/api': {
        changeOrigin: true,
        target: "http://localhost:5000/",
        //pathRewrite: {'^/api' : ''},
      },
    }
  },

};