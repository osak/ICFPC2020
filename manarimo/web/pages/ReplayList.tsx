import React, { useState } from "react";
import { Button, Container, FormGroup, Input, Row } from "reactstrap";
import { useHistory } from "react-router-dom";

export const ReplayList = () => {
  const history = useHistory();
  const [text, setText] = useState("");
  return (
    <Container>
      <Row>
        <FormGroup>
          <Input value={text} onChange={(e) => setText(e.target.value)} />
          <Button
            onClick={() => {
              history.push({ pathname: `/replay/${text}` });
            }}
          >
            Show
          </Button>
        </FormGroup>
      </Row>
    </Container>
  );
};
