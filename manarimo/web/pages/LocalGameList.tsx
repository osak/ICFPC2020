import { connect, PromiseState } from "react-refetch";
import { Link } from "react-router-dom";
import { Alert, Container, Row, Spinner, Table } from "reactstrap";
import { LocalRunResponse } from "../types";
import React from "react";

declare var API_BASE: string;
const LOCAL_RUNS_API = `${API_BASE}/local_runs`;

interface Props {
  localRunResponse: PromiseState<LocalRunResponse>;
}

const InnerLocalGameList = (props: Props) => {
  if (props.localRunResponse.pending) {
    return <Spinner />;
  }
  if (props.localRunResponse.rejected) {
    return <Alert color="danger">Server Error</Alert>;
  }
  const response = props.localRunResponse.value.items;
  response.sort((a, b) => b.timestamp.localeCompare(a.timestamp));

  return (
    <Container>
      <Table>
        <Row>
          <Table>
            <thead>
              <tr>
                <th>Attacker</th>
                <th>Defender</th>
                <th>Timestamp</th>
                <th>Visualize</th>
              </tr>
            </thead>
            <tbody>
              {response.map((entry) => (
                <tr key={entry.attacker_key}>
                  <td>{entry.attacker_name}</td>
                  <td>{entry.defender_name}</td>
                  <td>{entry.timestamp}</td>
                  <td>
                    <Link to={`/replay/${entry.attacker_key}`}>Go</Link>
                  </td>
                </tr>
              ))}
            </tbody>
          </Table>
        </Row>
      </Table>
    </Container>
  );
};

export const LocalGameList = connect<{}, Props>(() => ({
  localRunResponse: LOCAL_RUNS_API,
}))(InnerLocalGameList);
