import React, { useEffect, useRef, useState } from "react";

const TOOLTIP_W = 100;
const TOOLTIP_H = 50;

const renderTooltip = (
  ctx: CanvasRenderingContext2D,
  actualX: number,
  actualY: number,
  blockWidth: number,
  width: number,
  height: number,
  offsetX: number,
  offsetY: number
) => {
  const blockX = Math.floor(actualX / blockWidth);
  const blockY = Math.floor(actualY / blockWidth);

  const rectX =
    actualX + (3 * TOOLTIP_W) / 2 < width
      ? actualX + TOOLTIP_W / 2
      : actualX - (3 * TOOLTIP_W) / 2;
  const rectY = actualY + TOOLTIP_H < height ? actualY : actualY - TOOLTIP_H;

  ctx.fillStyle = "white";
  ctx.fillRect(rectX, rectY, TOOLTIP_W, TOOLTIP_H);
  ctx.beginPath();
  ctx.rect(rectX, rectY, TOOLTIP_W, TOOLTIP_H);
  ctx.stroke();

  ctx.font = "20px sans";
  ctx.fillStyle = "black";
  ctx.fillText(`x: ${blockX + offsetX}`, rectX + 10, rectY + TOOLTIP_H - 30);
  ctx.fillText(`y: ${blockY + offsetY}`, rectX + 10, rectY + TOOLTIP_H - 10);
};

interface Position {
  x: number;
  y: number;
}

interface Props {
  height: number;
  width: number;
  layers: [number, number][][];
  onClick?: (pos: { x: number; y: number }) => void;
}

export const CanvasBoard = (props: Props) => {
  const { height, width, layers } = props;
  const [mousePosition, setMousePosition] = useState<Position | undefined>(
    undefined
  );
  const canvasRef = useRef<HTMLCanvasElement>(null);

  const minX = Math.min(...layers.flatMap((layer) => layer).map(([x]) => x));
  const maxX = Math.max(...layers.flatMap((layer) => layer).map(([x]) => x));
  const minY = Math.min(...layers.flatMap((layer) => layer).map(([_, y]) => y));
  const maxY = Math.max(...layers.flatMap((layer) => layer).map(([_, y]) => y));
  const normalizedLayers = layers.map((l) =>
    l.map(([x, y]) => [x - minX, y - minY])
  );
  const maxW = Math.max(maxX - minX + 1, maxY - minY + 1);
  const blockWidth = Math.min(height, width) / maxW;

  useEffect(() => {
    const canvas: any = canvasRef.current;
    if (canvas) {
      const ctx: CanvasRenderingContext2D = canvas.getContext("2d");
      ctx.clearRect(0, 0, width, height);
      normalizedLayers.flat().forEach(([x, y]) => {
        ctx.fillStyle = "black";
        ctx.fillRect(x * blockWidth, y * blockWidth, blockWidth, blockWidth);
      });

      if (mousePosition) {
        const { x, y } = mousePosition;
        renderTooltip(ctx, x, y, blockWidth, width, height, minX, minY);
      }
    }
  });

  if (Math.max(...layers.map((p) => p.length)) === 0) {
    return <div />;
  }

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
        onClick={(e) => {
          const canvas = canvasRef.current;
          if (canvas) {
            const rect = canvas.getBoundingClientRect();
            const x = e.clientX - rect.left;
            const y = e.clientY - rect.top;
            if (props.onClick) {
              props.onClick({
                x: Math.floor(x / blockWidth) + minX,
                y: Math.floor(y / blockWidth) + minY,
              });
            }
          }
        }}
      />
    </div>
  );
};
