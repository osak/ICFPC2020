import React, { useState } from "react";
import { Container, Input, Row } from "reactstrap";
import { CanvasBoard } from "./CanvasBoard";

export const Visualizer = () => {
  const [text, setText] = useState("");

  const values = text
    .split(/[^0-9-]/)
    .filter((x) => x.length > 0)
    .map((x) => Number(x))
    .filter((t) => !isNaN(Number(t)));
  const points = [] as [number, number][];
  for (let i = 0; i + 1 < values.length; i += 2) {
    points.push([values[i], values[i + 1]]);
  }

  return (
    <Container>
      <Row>
        <CanvasBoard height={700} width={700} points={points} />
      </Row>
      <Row>
        <Input
          type="textarea"
          value={text}
          onChange={(e) => setText(e.target.value)}
        />
      </Row>
    </Container>
  );
};
