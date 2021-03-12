// To see this in action, run this in a terminal:
//      gp preview $(gp url 8000)

import * as React from "react";
import * as ReactDOM from "react-dom";
import { Api, JsonRpc, RpcError } from 'eosjs';
import { JsSignatureProvider } from 'eosjs/dist/eosjs-jssig';
import { Tabs, Radio, Space } from 'antd';
import 'antd/dist/antd.css';

const { TabPane } = Tabs;

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
    from?: string;
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
                from: 'integral',
                to: 'alice',
                quantity: "100.0000 INT",
                memo: "m",
            },
            error: '',
        };
    }

    setData(data: IssueData) {
        this.setState({ data: { ...this.state.data, ...data } });
    }

    async post() {
        try {
            const issue_data = {
                to: this.state.data.from,
                quantity: this.state.data.quantity,
                memo: this.state.data.memo,
            };
            this.api.signatureProvider = new JsSignatureProvider([this.state.privateKey]);
            const issue_result = await this.api.transact(
                {
                    actions: [{
                        account: 'integral',
                        name: 'issue',
                        authorization: [{
                            actor: this.state.data.from,
                            permission: 'active',
                        }],
                        data: issue_data,
                    }]
                }, {
                    blocksBehind: 3,
                    expireSeconds: 30,
                });
            const issue_transfer = await this.api.transact(
                {
                    actions: [{
                        account: 'integral',
                        name: 'transfer',
                        authorization: [{
                            actor: this.state.data.from,
                            permission: 'active',
                        }],
                        data: this.state.data,
                    }]
                }, {
                    blocksBehind: 3,
                    expireSeconds: 30,
                });
            console.log(issue_result);
            console.log(issue_transfer);
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
                        <td>From</td>
                        <td><input
                            style={{ width: 500 }}
                            value={this.state.data.from}
                            onChange={e => this.setData({ to: e.target.value })}
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

interface ConsumeData {
    from?: string;
    to?: string;
    quantity?: string;
    memo?: string;
};

interface ConsumeFormState {
    privateKey: string;
    data: ConsumeData;
    error: string;
};

class ConsumeForm extends React.Component<{}, ConsumeFormState>{
    api: Api;

    constructor(props: {}) {
        super(props);
        this.api = new Api({ rpc, signatureProvider: new JsSignatureProvider([]) });
        this.state = {
            privateKey: '5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3',
            data: {
                from: 'integral',
                to: 'alice',
                quantity: "50.0000 INT",
                memo: 'm',
            },
            error: '',
        };
    }

    setData(data: ConsumeData) {
        this.setState({ data: { ...this.state.data, ...data } });
    }

    async post() {
        try {
            this.api.signatureProvider = new JsSignatureProvider([this.state.privateKey]);
            const result = await this.api.transact(
                {
                    actions: [{
                        account: 'integral',
                        name: 'transfer',
                        authorization: [{
                            actor: this.state.data.from,
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
                        <td>From</td>
                        <td><input
                            style={{ width: 500 }}
                            value={this.state.data.from}
                            onChange={e => this.setData({ from: e.target.value })}
                        /></td>
                    </tr>
                    <tr>
                        <td>to</td>
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
            <button onClick={e => this.post()}>Consume</button>
            {this.state.error && <div>
                <br />
                Error:
                <code><pre>{this.state.error}</pre></code>
            </div>}
        </div>;
    }
}

interface ExchangeData {
    from?: string;
    to?: string;
    quantity?: string;
    exchanged?: string;
    exchange?: string;
    memo?: string;
};

interface ExchangeFormState {
    privateKey: string;
    data: ExchangeData;
    error: string;
};

class ExchangeForm extends React.Component<{}, ExchangeFormState>{
    api: Api;

    constructor(props: {}) {
        super(props);
        this.api = new Api({ rpc, signatureProvider: new JsSignatureProvider([]) });
        this.state = {
            privateKey: '5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3',
            data: {
                from: 'alice',
                to: 'integral',
                quantity: "25.0000",
                exchange: "INTA",
                exchanged: "INTB",
                memo: 'm',
            },
            error: '',
        };
    }

    setData(data: ExchangeData) {
        this.setState({ data: { ...this.state.data, ...data } });
    }

    async post() {
        try {
            const back_data = {
                from: this.state.data.from,
                to: this.state.data.to,
                quantity: this.state.data.quantity + this.state.data.exchange,
                memo: this.state.data.memo,
            };
            const get_data = {
                from: this.state.data.to,
                to: this.state.data.from,
                quantity: this.state.data.quantity + this.state.data.exchanged,
                memo: this.state.data.memo,
            };
            this.api.signatureProvider = new JsSignatureProvider([this.state.privateKey]);
            const back_result = await this.api.transact(
                {
                    actions: [{
                        account: 'integral',
                        name: 'transfer',
                        authorization: [{
                            actor: this.state.data.from,
                            permission: 'active',
                        }],
                        data: back_data,
                    }]
                }, {
                    blocksBehind: 3,
                    expireSeconds: 30,
                });
            const get_result = await this.api.transact(
                {
                    actions: [{
                        account: 'integral',
                        name: 'transfer',
                        authorization: [{
                            actor: this.state.data.to,
                            permission: 'active',
                        }],
                        data: get_data,
                    }]
                }, {
                    blocksBehind: 3,
                    expireSeconds: 30,
                });
            console.log(back_result);
            console.log(get_data);
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
                        <td>From</td>
                        <td><input
                            style={{ width: 500 }}
                            value={this.state.data.from}
                            onChange={e => this.setData({ from: e.target.value })}
                        /></td>
                    </tr>
                    <tr>
                        <td>to</td>
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
                        <td>exchange</td>
                        <td><input
                            style={{ width: 500 }}
                            value={this.state.data.exchange}
                            onChange={e => this.setData({ exchange: e.target.value })}
                        /></td>
                    </tr>
                    <tr>
                        <td>exchanged</td>
                        <td><input
                            style={{ width: 500 }}
                            value={this.state.data.exchanged}
                            onChange={e => this.setData({ exchanged: e.target.value })}
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
            <button onClick={e => this.post()}>Exchange</button>
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
                const rows = await rpc.get_currency_stats("integral","INT");
                let content ='INT\n';
                let row = rows["INT"];
                content +=
                        (row.supply + '').padEnd(12) +
                        (row.max_supply + '').padEnd(12) + '  ' +
                        row.issuer;
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
    <Tabs>
          <TabPane tab="Create" key="1">
            <CreateForm />
          </TabPane>
          <TabPane tab="Tab 2" key="2">
            <IssueForm />
          </TabPane>
          <TabPane tab="Tab 3" key="3">
            <ExchangeForm />
          </TabPane>
        </Tabs>,
    document.getElementById("tab")
);

