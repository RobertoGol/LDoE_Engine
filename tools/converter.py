import os
import struct
import math

def convert_obj_to_binary(obj_path, out_path):
    raw_vertices = []
    faces_indices = []
    
    with open(obj_path, 'r', encoding='utf-8', errors='ignore') as f:
        for line in f:
            parts = line.strip().split()
            if not parts:
                continue
            if parts[0] == 'v':
                try:
                    x, y, z = map(float, parts[1:4])
                    if (math.isnan(x) or math.isnan(y) or math.isnan(z) or
                        math.isinf(x) or math.isinf(y) or math.isinf(z) or
                        abs(x) > 1000.0 or abs(y) > 1000.0 or abs(z) > 1000.0):
                        raw_vertices.append(None)
                    else:
                        raw_vertices.append((x, z, y))
                except ValueError:
                    raw_vertices.append(None)
            elif parts[0] == 'f':
                face = []
                for token in parts[1:]:
                    v_str = token.split('/')[0]
                    if v_str:
                        try:
                            idx = int(v_str)
                            if idx < 0:
                                idx = len(raw_vertices) + idx + 1
                            face.append(idx)
                        except ValueError:
                            pass
                if len(face) >= 3:
                    for i in range(1, len(face) - 1):
                        faces_indices.append((face[0], face[i], face[i+1]))

    unique_vertices = []
    index_buffer = []
    vertex_map = {}

    for tri in faces_indices:
        valid_tri = True
        tri_new_indices = []
        for old_idx_1based in tri:
            zero_idx = old_idx_1based - 1
            if zero_idx < 0 or zero_idx >= len(raw_vertices) or raw_vertices[zero_idx] is None:
                valid_tri = False
                break
            v_pos = raw_vertices[zero_idx]
            if old_idx_1based not in vertex_map:
                vertex_map[old_idx_1based] = len(unique_vertices)
                unique_vertices.append(v_pos)
            tri_new_indices.append(vertex_map[old_idx_1based])
        
        if valid_tri and len(tri_new_indices) == 3:
            index_buffer.extend(tri_new_indices)

    with open(out_path, 'wb') as f:
        f.write(b'LDMS')
        f.write(struct.pack('I', len(unique_vertices)))
        for v in unique_vertices:
            f.write(struct.pack('fff', v[0], v[1], v[2]))
        f.write(struct.pack('I', len(index_buffer)))
        for idx in index_buffer:
            f.write(struct.pack('I', idx))

def batch_convert(root_dir):
    count = 0
    for root, dirs, files in os.walk(root_dir):
        for file in files:
            if file.endswith('.obj'):
                obj_file = os.path.join(root, file)
                bin_file = os.path.splitext(obj_file)[0] + '.mesh'
                convert_obj_to_binary(obj_file, bin_file)
                count += 1
    print(f"[CONVERTER] Успешно очищено и перепаковано: {count}")

if __name__ == '__main__':
    batch_convert("assets")
