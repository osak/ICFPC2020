import React, { useRef, useState } from "react";

const TOOLTIP_W = 100;
const TOOLTIP_H = 50;

const renderTooltip = (
  ctx: CanvasRenderingContext2D,
  x: number,
  y: number,
  blockWidth: number,
  width: number,
  height: number
) => {
  const blockX = Math.floor(x / blockWidth);
  const blockY = Math.floor(y / blockWidth);

  const rectX =
    x + (3 * TOOLTIP_W) / 2 < width
      ? x + TOOLTIP_W / 2
      : x - (3 * TOOLTIP_W) / 2;
  const rectY = y + TOOLTIP_H < height ? y : y - TOOLTIP_H;

  ctx.fillStyle = "white";
  ctx.fillRect(rectX, rectY, TOOLTIP_W, TOOLTIP_H);
  ctx.beginPath();
  ctx.rect(rectX, rectY, TOOLTIP_W, TOOLTIP_H);
  ctx.stroke();

  ctx.font = "20px sans";
  ctx.fillStyle = "black";
  ctx.fillText(`x: ${blockX}`, rectX + 10, rectY + TOOLTIP_H - 30);
  ctx.fillText(`y: ${blockY}`, rectX + 10, rectY + TOOLTIP_H - 10);
};

interface Position {
  x: number;
  y: number;
}

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
  }

  const [mousePosition, setMousePosition] = useState<Position | undefined>(
    undefined
  );
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const [ctx, setContext] = useState<CanvasRenderingContext2D | null>(null);
  React.useEffect(() => {
    if (canvasRef.current) {
      const renderCtx = canvasRef.current.getContext("2d");

      if (renderCtx) {
        setContext(renderCtx);
      }
    }

    // Draw
    if (ctx) {
      ctx.clearRect(0, 0, width, height);
      normalized.forEach(([x, y]) => {
        ctx.fillStyle = "black";
        ctx.fillRect(x * blockWidth, y * blockWidth, blockWidth, blockWidth);
      });

      if (mousePosition) {
        console.log(mousePosition);
        const { x, y } = mousePosition;
        renderTooltip(ctx, x, y, blockWidth, width, height);
      }
    }
  }, [ctx, points, mousePosition]);

  return (
    <div>
      <canvas
        width={width}
        height={height}
        ref={canvasRef}
        onMouseLeave={() => setMousePosition(undefined)}
        onMouseMove={(e) => {
          const canvas = canvasRef.current;
          if (canvas) {
            const rect = canvas.getBoundingClientRect();
            const x = e.clientX - rect.left;
            const y = e.clientY - rect.top;
            setMousePosition({ x, y });
          }
        }}
      />
    </div>
  );
};
