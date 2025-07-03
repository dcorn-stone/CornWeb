// script.js
const canvas = document.getElementById('game');
const ctx    = canvas.getContext('2d');

const gridSize = 20;         // number of cells per row/col
const cellSize = canvas.width / gridSize;

let snake = [{ x: 10, y: 10 }];  // initial snake: one cell at center
let dir   = { x: 0, y: 0 };      // current movement direction
let food  = randomCell();
let score = 0;

document.addEventListener('keydown', e => {
  // map arrow keys / WASD to direction
  if (e.key === 'ArrowUp'    && dir.y === 0) dir = { x: 0, y: -1 };
  if (e.key === 'ArrowDown'  && dir.y === 0) dir = { x: 0, y:  1 };
  if (e.key === 'ArrowLeft'  && dir.x === 0) dir = { x: -1, y: 0 };
  if (e.key === 'ArrowRight' && dir.x === 0) dir = { x:  1, y: 0 };
});

function randomCell() {
  return {
    x: Math.floor(Math.random() * gridSize),
    y: Math.floor(Math.random() * gridSize)
  };
}

function gameLoop() {
  // move snake
  const head = { x: snake[0].x + dir.x, y: snake[0].y + dir.y };
  // wrap around edges
  head.x = (head.x + gridSize) % gridSize;
  head.y = (head.y + gridSize) % gridSize;

  // check self-collision
  if (snake.some(seg => seg.x === head.x && seg.y === head.y)) {
    alert('Game Over! Score: ' + score);
    snake = [{ x: 10, y: 10 }];
    dir = { x: 0, y: 0 };
    score = 0;
    food = randomCell();
  }

  snake.unshift(head);

  // eating food?
  if (head.x === food.x && head.y === food.y) {
    score++;
    food = randomCell();
  } else {
    snake.pop(); // remove tail
  }

  // draw everything
  ctx.fillStyle = '#222';
  ctx.fillRect(0, 0, canvas.width, canvas.height);

  // food
  ctx.fillStyle = '#e74c3c';
  ctx.fillRect(food.x * cellSize, food.y * cellSize, cellSize, cellSize);

  // snake
  ctx.fillStyle = '#2ecc71';
  snake.forEach((seg, i) => {
    ctx.fillRect(seg.x * cellSize, seg.y * cellSize, cellSize - 1, cellSize - 1);
  });

  // score
  ctx.fillStyle = '#fff';
  ctx.fillText('Score: ' + score, 5, canvas.height - 5);
}

// run at ~10 FPS
setInterval(gameLoop, 100);

