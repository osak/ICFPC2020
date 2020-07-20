import React, { useState } from "react";
import { Container, FormGroup, Input, Label, Row, Table } from "reactstrap";
import { SavedGame, SavedGameList } from "../../types";
import { Link } from "react-router-dom";

const topPlayers = ["Pigimarl", "Unagi", "RGBTeam", "Manarimo"];
const participants = ["All", "with Manarimo", "Top Players Only"] as const;
type Participant = typeof participants[number];

const winners = ["All", "Manarimo wins", "Manarimo loses"] as const;
type Winner = typeof winners[number];

const OfficialVizLink = (props: { id: string }) => (
  <a
    href={`https://icfpcontest2020.github.io/#/visualize?game=${props.id}`}
    target="_blank"
  >
    Official
  </a>
);

const OfficialLogLink = (props: { id: string }) => (
  <a
    href={`https://icfpc2020-api.testkontur.ru/logs?logKey=${props.id}`}
    target="_blank"
  >
    STDOUT
  </a>
);

interface Props {
  list: SavedGameList;
}

const isSelected = (
  game: SavedGame,
  participant: Participant,
  winner: Winner
) => {
  if (
    participant === "with Manarimo" &&
    !game.attacker_team_name.includes("Manarimo") &&
    !game.defender_team_name.includes("Manarimo")
  ) {
    return false;
  }
  if (participant === "Top Players Only") {
    const topAttacker = topPlayers.some((name) =>
      game.attacker_team_name.includes(name)
    );
    const topDefender = topPlayers.some((name) =>
      game.defender_team_name.includes(name)
    );
    if (!topAttacker || !topDefender) {
      return false;
    }
  }

  const winTeam =
    game.winner === "Attacker"
      ? game.attacker_team_name
      : game.winner === "Defender"
      ? game.defender_team_name
      : "";
  const loseTeam =
    game.winner === "Attacker"
      ? game.defender_team_name
      : game.winner === "Defender"
      ? game.attacker_team_name
      : "";

  if (winner === "Manarimo wins" && !winTeam.includes("Manarimo")) {
    return false;
  }
  if (winner === "Manarimo loses" && !loseTeam.includes("Manarimo")) {
    return false;
  }
  return true;
};

export const GameSearchTable = (props: Props) => {
  const list = props.list.items;
  list.sort((a, b) => b.id - a.id);
  const [selection, setSelection] = useState<Participant>("All");
  const [winner, setWinner] = useState<Winner>("All");
  return (
    <Container fluid>
      <Row>
        <FormGroup>
          <Label>Participants</Label>
          <Input
            type="select"
            onSelect={(e) => console.log(e.currentTarget.value)}
          >
            {participants.map((p) => (
              <option key={p}>{p}</option>
            ))}
          </Input>
        </FormGroup>
        <FormGroup>
          <Label>Win/Lose</Label>
          <Input
            type="select"
            onSelect={(e) => console.log(e.currentTarget.value)}
          >
            {winners.map((p) => (
              <option key={p}>{p}</option>
            ))}
          </Input>
        </FormGroup>
      </Row>
      <Row>
        <Table>
          <thead>
            <tr>
              <th>#</th>
              <th>Viz</th>
              <th>Tournament</th>
              <th>Ticks</th>
              <th>Winner</th>
              <th>Attacker</th>
              <th>Defender</th>
              <th>Played at</th>
            </tr>
          </thead>
          <tbody>
            {list.map((game) => {
              if (!isSelected(game, selection, winner)) {
                return null;
              }
              return (
                <tr key={game.id}>
                  <th>{game.id}</th>
                  <td>
                    <OfficialVizLink id={game.game_id} />|
                    <Link to={`/relay/${game.attacker_player_key}`}>
                      Manarimo
                    </Link>
                  </td>
                  <td>{game.tournament_id}</td>
                  <td>{game.ticks}</td>
                  <td>{game.winner}</td>
                  <td>
                    {game.attacker_team_name} #{game.attacker_submission_id}
                    {game.attacker_debug_log && (
                      <OfficialLogLink id={game.attacker_debug_log} />
                    )}
                  </td>
                  <td>
                    {game.defender_team_name} #{game.defender_submission_id}{" "}
                    {game.defender_debug_log && (
                      <OfficialLogLink id={game.defender_debug_log} />
                    )}
                  </td>
                  <td>{new Date(game.played_at).toLocaleString()}</td>
                </tr>
              );
            })}
          </tbody>
        </Table>
      </Row>
    </Container>
  );
};
