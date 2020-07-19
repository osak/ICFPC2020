import React, { useEffect, useRef, useState } from "react";
import { Col, Container, Input, Row, Table } from "reactstrap";
import { RelayData } from "../../types";

const BOARD_SIZE = 500;

const drawPlanet = (
  ctx: CanvasRenderingContext2D,
  planetSize: number,
  magnify: (value: number) => number,
  normalize: (v: number) => number
) => {
  ctx.fillStyle = "olive";
  ctx.fillRect(
    normalize(-planetSize),
    normalize(-planetSize),
    magnify(2 * planetSize),
    magnify(2 * planetSize)
  );
};

interface Props {
  replayId: string;
  replayData: RelayData;
}

export const BattleViewer = (props: Props) => {
  const { replayData, replayId } = props;
  const totalTurns = replayData.data.details.turns.length;
  const [currentTurn, setCurrentTurn] = useState(0);
  const canvasRef = useRef<HTMLCanvasElement>(null);

  const turnData = replayData.data.details.turns[currentTurn];
  const planetSize = replayData.data.details.planet_size;
  const spaceSize = replayData.data.details.space_size;
  const magnify = (v: number) => (v * BOARD_SIZE) / (2 * spaceSize);
  const normalize = (v: number) => magnify(v) + BOARD_SIZE / 2;

  useEffect(() => {
    const canvas = canvasRef.current;
    if (canvas) {
      const ctx = canvas.getContext("2d");
      if (ctx) {
        ctx.clearRect(0, 0, BOARD_SIZE, BOARD_SIZE);
        drawPlanet(ctx, planetSize, magnify, normalize);

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
            ctx.fillStyle = "red";
            ctx.fill();
          });
        turnData.data
          .filter((data) => data.state.is_defender)
          .forEach((data) => {
            ctx.fillStyle = "blue";
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
        <h2>Battle #{replayId}</h2>
      </Row>
      <Row>
        <Col>
          <canvas
            style={{
              borderStyle: "solid",
              borderColor: "black",
              borderWidth: "1px",
            }}
            width={BOARD_SIZE}
            height={BOARD_SIZE}
            ref={canvasRef}
          />
        </Col>
        <Col>
          <Row>
            <Table>
              <tbody>
                <tr>
                  <th>Turn</th>
                  <td>{turnData.turn_id}</td>
                </tr>
              </tbody>
            </Table>
          </Row>
          {[true, false].map((is_attacker, i) => {
            const player = is_attacker ? "Attacker" : "Defender";
            return (
              <React.Fragment key={i}>
                <Row>
                  <h4>{player}</h4>
                  <Table>
                    <thead>
                      <tr>
                        <th>Position</th>
                        <th>Speed</th>
                        <th>Fuel</th>
                        <th>Attack</th>
                        <th>Cool Speed</th>
                        <th>HP</th>
                      </tr>
                    </thead>
                    <tbody>
                      {turnData.data
                        .filter(
                          (data) =>
                            data.state.is_attacker === is_attacker &&
                            data.state.is_defender !== is_attacker
                        )
                        .map((data, i) => {
                          return (
                            <tr key={i}>
                              <td>`"(${data.state.location.x}, ${data.state.location.y})"`</td>
                              <td>`"(${data.state.velocity.x}, ${data.state.velocity.y})"`</td>
                              <td>{data.state.parameters.fuel}</td>
                              <td>{data.state.parameters.attack}</td>
                              <td>{data.state.parameters.cool_speed}</td>
                              <td>{data.state.parameters.health}</td>
                            </tr>
                          );
                        })}
                    </tbody>
                  </Table>
                </Row>
              </React.Fragment>
            );
          })}
        </Col>
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
    </Container>
  );
};
