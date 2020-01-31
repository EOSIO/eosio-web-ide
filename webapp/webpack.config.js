module.exports = {
    mode: "development",
    devtool: "source-map",

    resolve: {
        extensions: [".js", ".ts", ".tsx"]
    },

    module: {
        rules: [
            {
                test: /\.ts(x?)$/,
                exclude: /node_modules/,
                use: [
                    {
                        loader: "ts-loader"
                    }
                ]
            },
            {
                enforce: "pre",
                test: /\.js$/,
                exclude: /node_modules/,
                loader: "source-map-loader"
            }
        ],
    },

    devServer: {
        overlay: true,
        host: '0.0.0.0',
        sockPort: 443,
        allowedHosts: ['localhost', '.gitpod.io'],
    },

    externals: {
        "react": "React",
        "react-dom": "ReactDOM"
    }
};
