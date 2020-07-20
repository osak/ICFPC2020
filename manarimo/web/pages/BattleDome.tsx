import React, { useState } from "react";
import { connect, PromiseState } from "react-refetch";
import { Alert, Container, Spinner, Table } from "reactstrap";
import { GamesResponse } from "../types";

declare var OFFICIAL_API_BASE: string;
declare var OFFICIAL_API_KEY: string;
const API_URL = `${OFFICIAL_API_BASE}/games/non-rating?take=200&apiKey=${OFFICIAL_API_KEY}`;

interface Props {
  response: PromiseState<GamesResponse>;
}

const InnerBattleDome = (props: Props) => {
  const [defenderMain, setDefenderMain] = useState(true);
  if (props.response.pending) {
    return <Spinner />;
  }
  if (props.response.rejected) {
    return <Alert color="danger">Server Error</Alert>;
  }

  const response = props.response.value.games.filter(
    (game) =>
      game.attacker.submissionId !== game.defender.submissionId &&
      game.winner !== "Nobody"
  );

  const resultMap = new Map<
    number,
    Map<number, { win: number; lose: number }>
  >();
  const totalCount = new Map<number, { win: number; lose: number }>();
  const teamNames = new Map<number, string>();

  response.forEach((game) => {
    const [main, sub] = defenderMain
      ? [game.defender, game.attacker]
      : [game.attacker, game.defender];
    const win = defenderMain === (game.winner === "Defender");
    if (!main.submissionId || !sub.submissionId) {
      return;
    }
    teamNames.set(main.submissionId, main.team.teamName);
    teamNames.set(sub.submissionId, sub.team.teamName);

    const mainResult =
      resultMap.get(main.submissionId) ??
      new Map<number, { win: number; lose: number }>();
    const currentResult = mainResult.get(sub.submissionId) ?? {
      win: 0,
      lose: 0,
    };
    if (win) {
      currentResult.win += 1;
    } else {
      currentResult.lose += 1;
    }
    mainResult.set(sub.submissionId, currentResult);
    resultMap.set(main.submissionId, mainResult);

    const mainTotalCount = totalCount.get(main.submissionId) ?? {
      win: 0,
      lose: 0,
    };
    if (win) {
      mainTotalCount.win += 1;
    } else {
      mainTotalCount.lose += 1;
    }
    totalCount.set(main.submissionId, mainTotalCount);
  });

  const sortedTotalResult = Array.from(totalCount)
    .map(([submissionId, count]) => ({ submissionId, count }))
    .filter((x) => x.count.win + x.count.lose >= 5)
    .sort((a, b) => {
      const ra = a.count.win / (a.count.win + a.count.lose);
      const rb = b.count.win / (b.count.win + b.count.lose);
      return rb - ra;
    });

  return (
    <Container fluid>
      <Table bordered>
        <thead>
          <tr>
            <th>{defenderMain ? "Defender" : "Attacker"}</th>
            {sortedTotalResult.map((ai) => {
              const teamName = teamNames.get(ai.submissionId) ?? "";
              return (
                <th key={ai.submissionId}>
                  {teamName.slice(0, 4)} {ai.submissionId}
                </th>
              );
            })}
            <th>Result</th>
          </tr>
        </thead>
        <tbody>
          {sortedTotalResult.map((mainAi) => {
            const teamName = teamNames.get(mainAi.submissionId) ?? "";
            return (
              <tr key={mainAi.submissionId}>
                <th>
                  {teamName.slice(0, 10)} {mainAi.submissionId}
                </th>
                {sortedTotalResult.map((sub) => {
                  const result = resultMap
                    .get(mainAi.submissionId)
                    ?.get(sub.submissionId);
                  const text = result ? `W:${result.win} L:${result.lose}` : "";
                  return <td key={sub.submissionId}>{text}</td>;
                })}
                <td>
                  {mainAi.count.win}/{mainAi.count.win + mainAi.count.lose} (
                  {Math.round(
                    (mainAi.count.win * 100) /
                      (mainAi.count.win + mainAi.count.lose)
                  )}
                  %)
                </td>
              </tr>
            );
          })}
        </tbody>
      </Table>
    </Container>
  );
};

export const BattleDome = connect<{}, Props>(() => ({
  response: API_URL,
}))(InnerBattleDome);
