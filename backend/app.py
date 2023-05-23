from flask import Flask, request, jsonify

app = Flask(__name__)

# maze = [
#     ['#', '#', '#', '#', '#', '#', '#'],
#     ['#', 'S', ' ', ' ', ' ', ' ', '#'],
#     ['#', ' ', '#', '#', '#', ' ', '#'],
#     ['#', ' ', ' ', ' ', '#', ' ', '#'],
#     ['#', '#', '#', '#', '#', ' ', '#'],
#     ['#', ' ', ' ', ' ', ' ', 'E', '#'],
#     ['#', '#', '#', '#', '#', '#', '#'],
# ]

# {
#     "maze": [["#", "#", "#", "#", "#", "#", "#"], ["#", "S", " ", " ", " ", " ", "#"], ["#", " ", "#", "#", "#", " ", "#"], ["#", " ", " ", " ", "#", " ", "#"], ["#", "#", "#", "#", "#", " ", "#"], ["#", " ", " ", " ", " ", "E", "#"], ["#", "#", "#", "#", "#", "#", "#"]]
# }

# class Graph:
#     def __init__(self):
#         self.adjacency_list = {}
    
#     def add_node(self, node):
#         if node not in self.adjacency_list:
#             self.adjacency_list[node] = []
    
#     def add_edge(self, node1, node2):
#         if node1 in self.adjacency_list and node2 in self.adjacency_list:
#             self.adjacency_list[node1].append(node2)
    
#     def get_neighbors(self, node):
#         if node in self.adjacency_list:
#             return self.adjacency_list[node]
#         else:
#             return []


# def map_maze():
#     map = Graph()
#     map.add_node()
    

#     while(junction == True):

def find_shortest_path(maze):
    start_pos = None
    end_pos = None
    for row in range(len(maze)):
        for col in range(len(maze[row])):
            if maze[row][col] == 'S':
                start_pos = (row, col)
            elif maze[row][col] == 'E':
                end_pos = (row, col)

    movements = [(-1, 0), (1, 0), (0, -1), (0, 1)]

    queue = [(start_pos, [])]

    while queue:
        current_pos, path = queue.pop(0)
        row, col = current_pos

        if current_pos == end_pos:
            return path + [current_pos]

        for movement in movements:
            new_row = row + movement[0]
            new_col = col + movement[1]

            if (
                0 <= new_row < len(maze) and
                0 <= new_col < len(maze[new_row]) and
                maze[new_row][new_col] != '#'
            ):
                queue.append(((new_row, new_col), path + [current_pos]))
                maze[new_row][new_col] = '#'

    return None

@app.route('/shortest_path', methods=['POST'])
def shortest_path():
    maze_data = request.get_json()
    path = find_shortest_path(maze_data.get("maze"))
    if path:
        return jsonify({'path': path})
    else:
        return jsonify({'error': 'No path found'})

if __name__ == '__main__':
    app.run(debug=True)
