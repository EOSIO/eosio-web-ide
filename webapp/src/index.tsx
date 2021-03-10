/// To see this in action, run this in a terminal:
//      gp preview $(gp url 8000)

import * as React from "react";
import * as ReactDOM from "react-dom";
import { Api, JsonRpc, RpcError } from 'eosjs';
import { JsSignatureProvider } from 'eosjs/dist/eosjs-jssig';

const rpc = new JsonRpc(''); // nodeos and web server are on same port

interface PostData {
    id?: number;
    user?: string;
    reply_to?: number;
    content?: string;
};

interface LikePostData {
    id? :number;
    user?: string;
    reply_to?: number;
    react?: number;
};

interface PostFormState {
    privateKey: string;
    data: PostData;
    likeData: LikePostData;
    error: string;
    likeError: string;
};


class PostForm extends React.Component<{}, PostFormState> {
    api: Api;

    constructor(props: {}) {
        super(props);
        this.api = new Api({ rpc, signatureProvider: new JsSignatureProvider([]) });
        this.state = {
            privateKey: '5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3',
            data: {
                id: 0,
                user: 'bob',
                reply_to: 0,
                content: 'This is a test',
            },
            likeData: {
                id: 0,
                user: 'bob',
                reply_to: 0,
                react: 1,
            },
            error: '',
            likeError: '',
        };
    }

    setData(data: PostData) {
        this.setState({ data: { ...this.state.data, ...data } });
    }

    setLikeData(likeData: LikePostData) {
        this.setState({ likeData: { ...this.state.likeData, ...likeData } });
    }

    async post() {
        try {
            this.api.signatureProvider = new JsSignatureProvider([this.state.privateKey]);
            const result = await this.api.transact(
                {
                    actions: [{
                        account: 'talk',
                        name: 'post',
                        authorization: [{
                            actor: this.state.data.user,
                            permission: 'active',
                        }],
                        data: this.state.data,
                    }]
                }, {
                    blocksBehind: 3,
                    expireSeconds: 30,
                });
            console.log(result);
            this.setState({ error: '' });
        } catch (e) {
            if (e.json)
                this.setState({ error: JSON.stringify(e.json, null, 4) });
            else
                this.setState({ error: '' + e });
        }
    }

    async like() {
        try {
            this.api.signatureProvider = new JsSignatureProvider([this.state.privateKey]);
            const result = await this.api.transact(
                {
                    actions: [{
                        account: 'talk',
                        name: 'likepost',
                        authorization: [{
                            actor: this.state.likeData.user,
                            permission: 'active',
                        }],
                        data: this.state.likeData,
                    }]
                }, {
                    blocksBehind: 3,
                    expireSeconds: 30,
                });
            console.log(result);
            this.setState({ likeError: '' });
        } catch (e) {
            if (e.json)
                this.setState({ likeError: JSON.stringify(e.json, null, 4) });
            else
                this.setState({ likeError: '' + e });
        }
    }

    render() {
        return <div>
            <table>
                <tbody>
                    <tr>
                        <td>Private Key</td>
                        <td><input
                            style={{ width: 500 }}
                            value={this.state.privateKey}
                            onChange={e => this.setState({ privateKey: e.target.value })}
                        /></td>
                    </tr>
                    <tr>
                        <td>User</td>
                        <td><input
                            style={{ width: 500 }}
                            value={this.state.data.user}
                            onChange={e => this.setData({ user: e.target.value })}
                        /></td>
                    </tr>
                    <tr>
                        <td>Reply To</td>
                        <td><input
                            style={{ width: 500 }}
                            value={this.state.data.reply_to}
                            onChange={e => this.setData({ reply_to: +e.target.value })}
                        /></td>
                    </tr>
                    <tr>
                        <td>Content</td>
                        <td><input
                            style={{ width: 500 }}
                            value={this.state.data.content}
                            onChange={e => this.setData({ content: e.target.value })}
                        /></td>
                    </tr>
                </tbody>
            </table>
            <br />
            <button onClick={e => this.post()}>Post</button>
            {this.state.error && <div>
                <br />
                Error:
                <code><pre>{this.state.error}</pre></code>
            </div>}

            <table>
                <tbody>
                    <tr>
                        <td>User</td>
                        <td><input
                            style={{ width: 500 }}
                            value={this.state.likeData.user}
                            onChange={e => this.setLikeData({ user: e.target.value })}
                        /></td>
                    </tr>
                    <tr>
                        <td>Reply To</td>
                        <td><input
                            style={{ width: 500 }}
                            value={this.state.likeData.reply_to}
                            onChange={e => this.setLikeData({ reply_to: +e.target.value })}
                        /></td>
                    </tr>
                    <tr>
                        <td>Reaction</td>
                        <td><input
                            style={{ width: 500 }}
                            value={this.state.likeData.react}
                            onChange={e => this.setLikeData({ react: +e.target.value })}
                        /></td>
                    </tr>
                </tbody>
            </table>
            <button onClick={e => this.like()}>Like</button>
            {this.state.likeError && <div>
                <br />
                Error:
                <code><pre>{this.state.likeError}</pre></code>
            </div>}
        </div>;
    }
}

class Messages extends React.Component<{}, { content: string }> {
    interval: number;

    constructor(props: {}) {
        super(props);
        this.state = { content: '///' };
    }

    componentDidMount() {
        this.interval = window.setInterval(async () => {
            try {
                const rows = await rpc.get_table_rows({
                    json: true, code: 'talk', scope: '', table: 'message', limit: 1000,
                });
                let content =
                    'id          reply_to      user          content                   \n' +
                    '=======================================================================\n';
                for (let row of rows.rows)
                    content +=
                        (row.id + '').padEnd(12) +
                        (row.reply_to + '').padEnd(12) + '  ' +
                        row.user.padEnd(14) +
                        row.content.padEnd(24) + '\n';
                this.setState({ content });
            } catch (e) {
                if (e.json)
                    this.setState({ content: JSON.stringify(e.json, null, 4) });
                else
                    this.setState({ content: '' + e });
            }

        }, 200);
    }

    componentWillUnmount() {
        clearInterval(this.interval);
    }

    render() {
        return <code><pre>{this.state.content}</pre></code>;
    }
}

class Likes extends React.Component<{}, { content: string }> {
    interval: number;

    constructor(props: {}) {
        super(props);
        this.state = { content: '///' };
    }

    componentDidMount() {
        this.interval = window.setInterval(async () => {
            try {
                let content =
                    'id          reply_to      user          reaction                   \n' +
                    '===================================================================\n';
                const likeRows = await rpc.get_table_rows({
                    json: true, code: 'talk', scope: '', table: 'likes', limit: 1000,
                });                   
                for (let row of likeRows.rows)
                    content +=
                        (row.id + '').padEnd(12) +
                        (row.reply_to + '').padEnd(12) + '  ' +
                        row.user.padEnd(14) +
                        (row.react + '').padEnd(14) + '\n';                    
                this.setState({ content });
            } catch (e) {
                if (e.json)
                    this.setState({ content: JSON.stringify(e.json, null, 4) });
                else
                    this.setState({ content: '' + e });
            }

        }, 200);
    }

    componentWillUnmount() {
        clearInterval(this.interval);
    }

    render() {
        return <code><pre>{this.state.content}</pre></code>;
    }
}

ReactDOM.render(
    <div>
        <PostForm />
        <br />
        Messages:
        <Messages />
        Likes:
        <Likes />
    </div>,
    document.getElementById("example")
);