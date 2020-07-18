import React, { useEffect, useState } from "react";
import {
  Container,
  Input,
  Pagination,
  PaginationItem,
  PaginationLink,
  Row,
} from "reactstrap";
import { parseImageString } from "../../util/ImageParser";
import { CanvasBoard } from "./CanvasBoard";
import { useHistory } from "react-router-dom";

declare var API_BASE: string;
const INTERACT_API = `${API_BASE}/interact`;

interface InteractiveState {
  state: string;
  data: string[];
}

const parseState = (json: string) => {
  const state = JSON.parse(json);
  const isState = (obj: any): obj is InteractiveState =>
    obj !== undefined &&
    obj !== null &&
    typeof obj.state === "string" &&
    Array.isArray(obj.data);
  console.log(state);
  if (isState(state)) {
    return state;
  } else {
    return undefined;
  }
};

export const Visualizer = () => {
  const history = useHistory();
  const json = new URLSearchParams(history.location.search).get("json");
  const interactiveState = json ? parseState(json) : undefined;
  const images = interactiveState?.data ?? [];
  const [text, setText] = useState("");
  const state = interactiveState?.state ?? "nil";

  useEffect(() => {
    if (images.length > 0) {
      setText(images[0]);
    }
  }, [json]);

  return (
    <Container>
      <Row>
        <CanvasBoard
          height={700}
          width={1000}
          points={parseImageString(text)}
          onClick={(pos) => {
            fetch(INTERACT_API, {
              method: "POST",
              body: JSON.stringify({
                state,
                data: `(${pos.x},${pos.y})`,
              }),
            })
              .then((response) => response.json())
              .then((response) => {
                const nextState = JSON.stringify(response);
                const query = new URLSearchParams();
                query.set("json", nextState);
                history.push({ search: query.toString() });
              });
          }}
        />
      </Row>
      {images.length && (
        <Pagination>
          {images.map((image, i) => (
            <PaginationItem key={i}>
              <PaginationLink onClick={() => setText(images[i])}>
                {i + 1}
              </PaginationLink>
            </PaginationItem>
          ))}
        </Pagination>
      )}
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
