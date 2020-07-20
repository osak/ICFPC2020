import React, { useState } from "react";
import {
  Badge,
  Button,
  Container,
  FormGroup,
  Input,
  Label,
  Row,
  Table,
} from "reactstrap";
import { SavedGame, SavedGameList } from "../../types";
import { Link } from "react-router-dom";

const topPlayers = ["Pigimarl", "Unagi", "RGBTeam", "Manarimo"];
const participants = ["All", "Top Players Only"] as const;
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
  return !(winner === "Manarimo loses" && !loseTeam.includes("Manarimo"));
};

export const GameSearchTable = (props: Props) => {
  const list = props.list.items;
  list.sort((a, b) => b.played_at.localeCompare(a.played_at));
  const [selection, setSelection] = useState<Participant>("All");
  const [winner, setWinner] = useState<Winner>("All");
  const [searchText, setSearchText] = useState("");
  const [showCount, setShowCount] = useState(200);
  return (
    <Container fluid>
      <Row>
        <FormGroup>
          <Label>Participants</Label>
          <Input
            type="select"
            value={selection}
            onChange={(e) => {
              const option = participants.find((a) => a === e.target.value);
              if (option) {
                setSelection(option);
              }
            }}
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
            value={winner}
            onChange={(e) => {
              const option = winners.find((a) => a === e.target.value);
              if (option) {
                setWinner(option);
              }
            }}
          >
            {winners.map((p) => (
              <option key={p}>{p}</option>
            ))}
          </Input>
        </FormGroup>
        <FormGroup>
          <Label>Search</Label>
          <Input
            type="text"
            onChange={(e) => setSearchText(e.target.value)}
            value={searchText}
          />
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
            {list
              .filter((game) => isSelected(game, selection, winner))
              .slice(0, showCount)
              .map((game) => {
                if (
                  searchText.length > 0 &&
                  !JSON.stringify(game).includes(searchText)
                ) {
                  return null;
                }

                return (
                  <tr key={game.id}>
                    <th>{game.id}</th>
                    <td>
                      <OfficialVizLink id={game.game_id} /> |{" "}
                      <Link to={`/replay/${game.attacker_player_key}`}>
                        Manarimo
                      </Link>
                    </td>
                    <td>{game.tournament_id}</td>
                    <td>{game.ticks}</td>
                    <td>{game.winner}</td>
                    <td>
                      {game.winner === "Attacker" && (
                        <Badge color="success">Win</Badge>
                      )}{" "}
                      {game.attacker_team_name} #{game.attacker_submission_id}{" "}
                      {game.attacker_debug_log && (
                        <OfficialLogLink id={game.attacker_debug_log} />
                      )}
                    </td>
                    <td>
                      {game.winner === "Defender" && (
                        <Badge color="success">Win</Badge>
                      )}{" "}
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
      <Row>
        <Button onClick={() => setShowCount(showCount + 200)}>More</Button>
      </Row>
    </Container>
  );
};
