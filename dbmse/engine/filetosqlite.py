#!/usr/bin/env python3
import sys
import sqlite3

_, fin, fout, tbl = sys.argv

with open(fin, "r") as f:
    fcnt = int(f.readline())
    fnames = f.readline().split()
    ftys = f.readline().split()
    assert fcnt == len(fnames) == len(ftys)

    fields = list(map(lambda x: x[0] + " " + x[1], zip(fnames, ftys)))

    with sqlite3.connect(fout) as conn:
        c = conn.cursor()
        c.execute("DROP TABLE IF EXISTS {}".format(tbl))
        c.execute("CREATE TABLE {} ({})".format(tbl, ",".join(fields)))

        templ = "INSERT INTO {} VALUES ({})".format(tbl, ",".join("?" for _ in range(fcnt)))
        for l in f:
            names = l.split()
            assert len(names) == fcnt
            c.execute(templ, names)
