from flask import Flask, request, jsonify

app = Flask(__name__)

# maze = [
#     ['#', '#', '#', '#', '#', '#', '#'],
#     ['#', 'S', '#', ' ', ' ', ' ', '#'],
#     ['#', ' ', '#', '#', '#', ' ', '#'],
#     ['#', ' ', ' ', ' ', '#', ' ', '#'],
#     ['#', '#', '#', '#', '#', ' ', '#'],
#     ['#', ' ', ' ', ' ', ' ', 'E', '#'],
#     ['#', '#', '#', '#', '#', '#', '#'],
# ]

def find_shortest_path(maze):
    # Initialize the start and end positions
    start_pos = None
    end_pos = None
    for row in range(len(maze)):
        for col in range(len(maze[row])):
            if maze[row][col] == 'S':
                start_pos = (row, col)
            elif maze[row][col] == 'E':
                end_pos = (row, col)

    # Define the movements: up, down, left, right
    movements = [(-1, 0), (1, 0), (0, -1), (0, 1)]

    # Queue to store the positions and their corresponding paths
    queue = [(start_pos, [])]

    while queue:
        current_pos, path = queue.pop(0)
        row, col = current_pos

        # Reached the end position
        if current_pos == end_pos:
            return path + [current_pos]

        # Explore the neighboring positions
        for movement in movements:
            new_row = row + movement[0]
            new_col = col + movement[1]

            # Check if the new position is within the maze boundaries and not a wall
            if (
                0 <= new_row < len(maze) and
                0 <= new_col < len(maze[new_row]) and
                maze[new_row][new_col] != '#'
            ):
                # Add the new position and path to the queue
                queue.append(((new_row, new_col), path + [current_pos]))
                # Mark the position as visited by replacing it with '#'
                maze[new_row][new_col] = '#'

    # No path found
    return None

@app.route('/shortest_path', methods=['POST'])
def shortest_path():
    maze_data = request.get_json()
    path = find_shortest_path(maze_data)
    if path:
        return jsonify({'path': path})
    else:
        return jsonify({'error': 'No path found'})

if __name__ == '__main__':
    app.run(debug=True)
