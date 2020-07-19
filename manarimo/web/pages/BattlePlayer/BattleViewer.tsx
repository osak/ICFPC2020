import React, { useEffect, useRef, useState } from "react";
import { Col, Container, Input, Row, Table } from "reactstrap";
import { RelayData } from "../../types";
import { parseCommand } from "../../util/CommandParser";

interface Location {
  x: number;
  y: number;
}

const BOARD_SIZE = 500;

const drawAttack = (
  ctx: CanvasRenderingContext2D,
  from: Location,
  to: Location,
  normalize: (pos: number) => number
) => {
  ctx.beginPath();
  ctx.strokeStyle = "lime";
  ctx.moveTo(normalize(from.x), normalize(from.y));
  ctx.lineTo(normalize(to.x), normalize(to.y));
  ctx.stroke();
};

const drawPlayer = (
  ctx: CanvasRenderingContext2D,
  location: Location,
  color: string,
  normalize: (pos: number) => number,
  magnify: (v: number) => number
) => {
  ctx.beginPath();
  ctx.arc(
    normalize(location.x),
    normalize(location.y),
    magnify(1),
    0,
    2 * Math.PI
  );
  ctx.fillStyle = color;
  ctx.fill();
};

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

        turnData.data.forEach((data) => {
          const playerColor = data.state.is_attacker
            ? "red"
            : data.state.is_defender
            ? "blue"
            : "black";
          drawPlayer(ctx, data.state.location, playerColor, normalize, magnify);
          const command = parseCommand(data.command);
          if (command) {
            if (command.command === "ATTACK") {
              drawAttack(
                ctx,
                data.state.location,
                { x: command.x, y: command.y },
                normalize
              );
            }
          }
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
                        <th>Fuel</th>
                        <th>Attack</th>
                        <th>Cooler</th>
                        <th>HP</th>
                        <th>Command</th>
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
                          const command = parseCommand(data.command);

                          return (
                            <tr key={i}>
                              <td>{data.state.parameters.fuel}</td>
                              <td>{data.state.parameters.attack}</td>
                              <td>{data.state.parameters.cool_speed}</td>
                              <td>{data.state.parameters.health}</td>
                              <td>
                                <p>{JSON.stringify(data.command)}</p>
                                <p>{command ? command.command : "null"}</p>
                              </td>
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
