import React, { useRef, useState } from "react";

interface Props {
  height: number;
  width: number;
  points: [number, number][];
}

export const CanvasBoard = (props: Props) => {
  const { height, width, points } = props;

  let normalized = [] as [number, number][];
  let blockWidth = 1;
  if (points.length !== 0) {
    const minX = Math.min(...points.map(([x]) => x));
    const maxX = Math.max(...points.map(([x]) => x));
    const minY = Math.min(...points.map(([_, y]) => y));
    const maxY = Math.max(...points.map(([_, y]) => y));

    normalized = points.map(([x, y]) => [x - minX, y - minY]);
    const maxW = Math.max(maxX - minX + 1, maxY - minY + 1);
    blockWidth = Math.min(height, width) / Math.max(maxW, 1);
    console.log({ normalized, blockWidth, maxW, minX, minY, points });
  }

  const canvasRef = useRef<HTMLCanvasElement>(null);
  const [context, setContext] = useState<CanvasRenderingContext2D | null>(null);
  React.useEffect(() => {
    if (canvasRef.current) {
      const renderCtx = canvasRef.current.getContext("2d");

      if (renderCtx) {
        setContext(renderCtx);
      }
    }

    // Draw
    if (context) {
      context.clearRect(0, 0, width, height);
      normalized.forEach(([x, y]) => {
        context.fillStyle = "black";
        context.fillRect(
          x * blockWidth,
          y * blockWidth,
          blockWidth,
          blockWidth
        );
      });
    }
  }, [context, points]);

  return (
    <div>
      <canvas width={width} height={height} ref={canvasRef} />
    </div>
  );
};
