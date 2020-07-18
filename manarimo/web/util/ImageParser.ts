export const parseImageString = (text: string) => {
  const values = text
    .split(/[^0-9-]/)
    .filter((x) => x.length > 0)
    .map((x) => Number(x))
    .filter((t) => !isNaN(Number(t)));
  const points = [] as [number, number][];
  for (let i = 0; i + 1 < values.length; i += 2) {
    points.push([values[i], values[i + 1]]);
  }
  return points;
};
