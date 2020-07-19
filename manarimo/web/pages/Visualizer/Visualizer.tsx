import React, { useEffect, useState } from "react";
import { Button, ButtonGroup, Container, Input, Label, Row } from "reactstrap";
import { CanvasBoard } from "../../components/CanvasBoard";
import { parseImageString } from "../../util/ImageParser";
import { useHistory } from "react-router-dom";

declare var API_BASE: string;
const INTERACT_API = `${API_BASE}/interact`;

interface InteractiveState {
  state: string;
  data: string[];
  sends: string[];
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
  const [disableImages, setDisableImages] = useState<number[]>([]);
  const [state, setState] = useState(interactiveState?.state ?? "nil");
  const sends = interactiveState?.sends ?? [];
  const [message, setMessage] = useState("");

  useEffect(() => {
    setState(interactiveState?.state ?? "nil");
    setDisableImages([]);
  }, [json]);

  const imageText =
    "" + images.filter((_, i) => !disableImages.includes(i)).join();

  return (
    <Container>
      <Row>
        <CanvasBoard
          height={700}
          width={1000}
          layers={images.map((image, i) =>
            disableImages.includes(i) ? [] : parseImageString(image)
          )}
          onClick={(pos) => {
            setMessage("Synchronizing ...");
            fetch(INTERACT_API, {
              method: "POST",
              headers: {
                "Content-Type": "application/json",
              },
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
              })
              .then(() => setMessage(""));
          }}
        />
      </Row>
      <Row className="my-2">
        {images.length && (
          <ButtonGroup>
            {images.map((image, i) => (
              <Button
                key={i}
                onClick={() => {
                  if (disableImages.includes(i)) {
                    setDisableImages(disableImages.filter((j) => i !== j));
                  } else {
                    setDisableImages([...disableImages, i]);
                  }
                }}
                active={!disableImages.includes(i)}
              >
                {i + 1}
              </Button>
            ))}
          </ButtonGroup>
        )}
        {message}
      </Row>
      <Row className="my-2">
        <Label>Image</Label>
        <Input type="textarea" value={imageText} disabled />
      </Row>
      <Row className="my-2">
        <Label>State</Label>
        <Input
          type="textarea"
          value={state}
          onChange={(e) => setState(e.target.value)}
        />
      </Row>
      <Row className="my-2">
        <Label>Sends</Label>
        <Input
          type="textarea"
          value={sends.join("\n")}
          disabled
          onChange={(e) => setState(e.target.value)}
        />
      </Row>
    </Container>
  );
};
