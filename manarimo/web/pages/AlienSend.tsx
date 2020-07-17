import React, { useState } from "react";
import { connect, PromiseState } from "react-refetch";
import { Button, Container, Input, Row } from "reactstrap";
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
  const response = props.response.value?.response;

  return (
    <Container>
      <Row>
        <Input value={text} onChange={(e) => setText(e.target.value)} />
      </Row>
      <Row>
        <Button
          disabled={text.length === 0 || sending}
          onClick={() => {
            props.submit(text);
          }}
        >
          {sending ? "Sending" : "Send"}
        </Button>
      </Row>
      <Row> {response ? `Response: ${response}` : ""}</Row>
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
