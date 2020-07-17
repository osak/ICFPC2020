import React from "react";
import { connect, PromiseState } from "react-refetch";
import { Container, Row, Table } from "reactstrap";
import { SendHistory } from "../types";

declare var API_BASE: string;
const HISTORY_API = `${API_BASE}/send_history`;

interface InnerProps {
  historyResponse: PromiseState<SendHistory>;
}

const InnerHistoryList = (props: InnerProps) => {
  const history = props.historyResponse.fulfilled
    ? props.historyResponse.value.items
    : [];

  history.sort((a, b) => b.timestamp.localeCompare(a.timestamp));
  return (
    <Container>
      <Row>
        <Table>
          <thead>
            <tr>
              <th>#</th>
              <th>Request</th>
              <th>Response</th>
              <th>Time</th>
            </tr>
          </thead>
          <tbody>
            {history.map((entry) => (
              <tr key={entry.id}>
                <th>{entry.id}</th>
                <td>{entry.request}</td>
                <td>{entry.response}</td>
                <td>{entry.timestamp}</td>
              </tr>
            ))}
          </tbody>
        </Table>
      </Row>
    </Container>
  );
};
export const HistoryList = connect<{}, InnerProps>(() => ({
  historyResponse: HISTORY_API,
}))(InnerHistoryList);
