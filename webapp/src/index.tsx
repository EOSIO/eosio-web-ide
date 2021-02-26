// To see this in action, run this in a terminal:
//      gp preview $(gp url 8000)

import * as React from "react";
import * as ReactDOM from "react-dom";
import { Api, JsonRpc, RpcError } from 'eosjs';
import { JsSignatureProvider } from 'eosjs/dist/eosjs-jssig';

const rpc = new JsonRpc(''); // nodeos and web server are on same port

interface CreateData {
    issuer?: string;
    maximum_supply?: string;
};

interface CreateFormState {
    privateKey: string;
    data: CreateData;
    error: string;
};

class CreateForm extends React.Component<{}, CreateFormState> {
    api: Api;

    constructor(props: {}) {
        super(props);
        this.api = new Api({ rpc, signatureProvider: new JsSignatureProvider([]) });
        this.state = {
            privateKey: '5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3',
            data: {
                issuer: 'integral',
                maximum_supply: "1000000.0000 INT"
            },
            error: '',
        };
    }

    setData(data: CreateData) {
        this.setState({ data: { ...this.state.data, ...data } });
    }

    async post() {
        try {
            this.api.signatureProvider = new JsSignatureProvider([this.state.privateKey]);
            const result = await this.api.transact(
                {
                    actions: [{
                        account: 'integral',
                        name: 'create',
                        authorization: [{
                            actor: 'integral',
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
                        <td>Issuer</td>
                        <td><input
                            style={{ width: 500 }}
                            value={this.state.data.issuer}
                            onChange={e => this.setData({ issuer: e.target.value })}
                        /></td>
                    </tr>
                    <tr>
                        <td>Maximum_supply</td>
                        <td><input
                            style={{ width: 500 }}
                            value={this.state.data.maximum_supply}
                            onChange={e => this.setData({ maximum_supply: e.target.value })}
                        /></td>
                    </tr>
                </tbody>
            </table>
            <br />
            <button onClick={e => this.post()}>Create</button>
            {this.state.error && <div>
                <br />
                Error:
                <code><pre>{this.state.error}</pre></code>
            </div>}
        </div>;
    }
}

interface IssueData {
    to?: string;
    quantity?: string;
    memo?: string;
};

interface IssueFormState {
    privateKey: string;
    data: IssueData;
    error: string;
};

class IssueForm extends React.Component<{}, IssueFormState> {
    api: Api;

    constructor(props: {}) {
        super(props);
        this.api = new Api({ rpc, signatureProvider: new JsSignatureProvider([]) });
        this.state = {
            privateKey: '5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3',
            data: {
                to: 'alice',
                quantity: "100.0000 INT",
                memo: "m"
            },
            error: '',
        };
    }

    setData(data: IssueData) {
        this.setState({ data: { ...this.state.data, ...data } });
    }

    async post() {
        try {
            this.api.signatureProvider = new JsSignatureProvider([this.state.privateKey]);
            const result = await this.api.transact(
                {
                    actions: [{
                        account: this.state.data.to,
                        name: 'create',
                        authorization: [{
                            actor: this.state.data.to,
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
                        <td>To</td>
                        <td><input
                            style={{ width: 500 }}
                            value={this.state.data.to}
                            onChange={e => this.setData({ to: e.target.value })}
                        /></td>
                    </tr>
                    <tr>
                        <td>quantity</td>
                        <td><input
                            style={{ width: 500 }}
                            value={this.state.data.quantity}
                            onChange={e => this.setData({ quantity: e.target.value })}
                        /></td>
                    </tr>
                    <tr>
                        <td>Content</td>
                        <td><input
                            style={{ width: 500 }}
                            value={this.state.data.memo}
                            onChange={e => this.setData({ memo: e.target.value })}
                        /></td>
                    </tr>
                </tbody>
            </table>
            <br />
            <button onClick={e => this.post()}>Issue</button>
            {this.state.error && <div>
                <br />
                Error:
                <code><pre>{this.state.error}</pre></code>
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
                    'id          reply_to      user          content\n' +
                    '=============================================================\n';
                for (let row of rows.rows)
                    content +=
                        (row.id + '').padEnd(12) +
                        (row.reply_to + '').padEnd(12) + '  ' +
                        row.user.padEnd(14) +
                        row.content + '\n';
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
        <CreateForm />
        <br />
        <IssueForm />
        <br />
        Messages:
        <Messages />
    </div>,
    document.getElementById("example")
);
