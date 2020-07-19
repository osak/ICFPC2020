import React, { useEffect, useRef, useState } from "react";
import { Container, Input, Row } from "reactstrap";
import { RelayData } from "../../types";

const BOARD_SIZE = 500;

const drawPlanet = (
  ctx: CanvasRenderingContext2D,
  planetSize: number,
  spaceSize: number,
  magnify: (value: number) => number
) => {
  const center = spaceSize / 2;
  ctx.fillStyle = "olive";
  ctx.fillRect(
    magnify(center - planetSize),
    magnify(center - planetSize),
    magnify(2 * planetSize),
    magnify(2 * planetSize)
  );
};

interface Props {
  replayData: RelayData;
}

export const BattleViewer = (props: Props) => {
  const { replayData } = props;
  const totalTurns = replayData.data.details.turns.length;
  const [currentTurn, setCurrentTurn] = useState(0);
  const canvasRef = useRef<HTMLCanvasElement>(null);

  const turnData = replayData.data.details.turns[currentTurn];
  const planetSize = replayData.data.details.planet_size;
  const spaceSize = replayData.data.details.space_size;
  const normalize = (v: number) =>
    (v * BOARD_SIZE) / spaceSize + BOARD_SIZE / 2;
  const magnify = (v: number) => (v * BOARD_SIZE) / spaceSize;

  useEffect(() => {
    const canvas = canvasRef.current;
    if (canvas) {
      const ctx = canvas.getContext("2d");
      if (ctx) {
        ctx.clearRect(0, 0, BOARD_SIZE, BOARD_SIZE);
        drawPlanet(ctx, planetSize, spaceSize, magnify);

        turnData.data
          .filter((data) => data.state.is_attacker)
          .forEach((data) => {
            ctx.beginPath();
            ctx.arc(
              normalize(data.state.location.x),
              normalize(data.state.location.y),
              magnify(1),
              0,
              2 * Math.PI
            );
            ctx.fillStyle = "maroon";
            ctx.fill();
          });
        turnData.data
          .filter((data) => data.state.is_defender)
          .forEach((data) => {
            ctx.fillStyle = "navy";
            ctx.fillRect(
              normalize(data.state.location.x) - magnify(1),
              normalize(data.state.location.y) - magnify(1),
              magnify(2),
              magnify(2)
            );
          });
      }
    }
  });

  return (
    <Container>
      <Row>
        <canvas width={BOARD_SIZE} height={BOARD_SIZE} ref={canvasRef} />
      </Row>
      <Row>
        <Input
          type="range"
          min="0"
          max={totalTurns - 1}
          value={currentTurn}
          onChange={(e) => setCurrentTurn(parseInt(e.target.value))}
        />
      </Row>
      <Row>Turn: {turnData.turn_id}</Row>
    </Container>
  );
};
