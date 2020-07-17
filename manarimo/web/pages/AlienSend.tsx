import React, { useState } from "react";
import { connect, PromiseState } from "react-refetch";
import { Alert, Button, Container, Input, Row, Table } from "reactstrap";
import { SubmitResponse } from "../types";

declare var API_BASE: string;
const SUBMIT_API = `${API_BASE}/send`;

interface InnerProps {
  submit: (payload: string) => void;
  response: PromiseState<SubmitResponse | null>;
}

const InnerAlienSend = (props: InnerProps) => {
  const [text, setText] = useState("");
  const sending = props.response.pending || props.response.refreshing;
  const rejected = props.response.rejected;
  const response = props.response.value;

  return (
    <Container>
      <Row>
        <Input value={text} onChange={(e) => setText(e.target.value)} />
      </Row>
      <Row className="my-2">
        <Button
          disabled={text.length === 0 || sending}
          onKeyDown={(e) => {
            if (e.key === "Enter") {
              props.submit(text);
            }
          }}
          onClick={() => {
            props.submit(text);
          }}
        >
          {sending ? "Sending" : "Send"}
        </Button>
      </Row>
      {response && (
        <Row className="my-2">
          <Table>
            <tbody>
              <tr>
                <th>Request</th>
                <td>{response.raw_request}</td>
              </tr>
              <tr>
                <th>Raw Request</th>
                <td>{response.raw_request}</td>
              </tr>
              <tr>
                <th>Response</th>
                <td>{response.response}</td>
              </tr>
              <tr>
                <th>Raw Response</th>
                <td>{response.raw_response}</td>
              </tr>
              <tr>
                <th>Timestamp</th>
                <td>{response.timestamp}</td>
              </tr>
            </tbody>
          </Table>
        </Row>
      )}
      {rejected && (
        <Row className="my-2">
          <Alert color="danger">Server error</Alert>
        </Row>
      )}
    </Container>
  );
};

export const AlienSend = connect<{}, InnerProps>(() => ({
  submit: (payload) => ({
    response: {
      url: SUBMIT_API,
      method: "POST",
      body: JSON.stringify({
        payload,
      }),
    },
  }),
  response: { value: null },
}))(InnerAlienSend);
