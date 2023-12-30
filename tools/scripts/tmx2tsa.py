"""
Extension of Snakey1's tmx2tsa to incorporate palette IDs for each tile.

https://feuniverse.us/t/snakes-pit-of-asm-and-other-stuff/3627/84
https://github.com/Snakey11/SnekASM/blob/7cc65698eea9f0a8ac553226838c108bcfb35e65/BuildfileTools/tmx2tsa/tmx2tsa.py

Expects the TMX file to be layered according to the palette ID, e.g. with layers named as 'Layer_Palette_##'
"""

import argparse
import tmx
import struct
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument('file', help='TMX file to parse.')
parser.add_argument('output', help='Output dump file.')
parser.add_argument('-c', '--compress', help='Filepath to compress.exe for lz77 compression.')
args = parser.parse_args()

tsa = [] # List of shorts to write.
class TSA:
    def __init__(self, width, height, tiles):
        self.width = width # Width is the length per list.
        self.height = height
        self.tiles = [ tiles[i:i+self.width] for i in range(0, self.width * self.height, self.width) ] # This sorts the rows in reverse order.
        self.tiles.reverse()
    def write(self, file): # Output width-1, height-1, and then the list of struct unsigned halfwords.
        with open(file, 'wb') as out:
            out.write(bytes([self.width-1, self.height-1]))
            for row in self.tiles:
                for tile in row:
                    out.write(struct.pack('<H', (tile.gid-1) | (tile.hflip<<10) | (tile.vflip<<11) | (tile.palette_id<<12)))
    def __str__(self):
        return f'Width: {self.width}\nHeight: {self.height}\nTiles:\nself.tiles'

class Tile:
    def __init__(self, gid, hflip, vflip, palette_id):
        self.gid = gid
        self.hflip = hflip
        self.vflip = vflip
        self.palette_id = palette_id

if __name__ == '__main__':
    map = tmx.TileMap.load(args.file)

    if map.tilewidth != 8 or map.tileheight != 8:
        exit(f'Error in parsing {args.file}: Tile width and height both must be 8.')

    layers = [layer for layer in map.layers if layer.name.startswith("Layer_Palette_")]

    tiles = []
    for i in range(0, map.width * map.height):
        for layer in layers:
            tile = layer.tiles[i]
            if tile.gid != 0:
                palette_id = int(layer.name.split('_')[-1])
                tiles.append(Tile(tile.gid, tile.hflip, tile.vflip, palette_id))

    tsa = TSA(map.width, map.height, tiles)
    tsa.write(args.output)

    if args.compress:
        compressed = subprocess.run([args.compress,args.output], capture_output=True).stdout
        with open(args.output, 'wb') as out:
            out.write(compressed)
