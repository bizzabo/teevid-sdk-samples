const path = require('path')
const webpack = require('webpack')
const HtmlPlugin = require('html-webpack-plugin')
const ExtractTextPlugin = require('extract-text-webpack-plugin')

const config = {
    entry: './src/main-entry.js',
    output: {
        path: path.resolve(__dirname, 'views/dist'),
        filename: 'bundle.js'
    },
    module: {
        rules: [
            { test: /\.(png|woff|woff2|eot|ttf|svg|gif)$/, loader: 'url-loader?limit=100000' },
            {
                test: /\.css$/,
                use: ExtractTextPlugin.extract({
                    use: 'css-loader', fallback: 'style-loader'
                })
            },
            {
                test: /\.js$/,
                loader: 'babel-loader',
                exclude: /node_modules/
            }
        ]
    },
    plugins: [
        new ExtractTextPlugin('bundle.css'),
        new webpack.optimize.UglifyJsPlugin({
            include: /\.min\.js$/,
            minimize: true
        })
    ]
}

module.exports = config