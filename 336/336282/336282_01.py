def extendedSig(seq, key, n):
    key = eval(key)
    top = seq.index(n - 1)
    attachement = top - 1
    for i in range(attachement, -1, -1):
        if key[i] > 0:
            key[i] -= 1
            key.insert(top, 2)
            return key

e_list = [{"[2]": [[0], 1]}, {}]

def A(n):
    if n < 2:
        return 1
    el_0 = e_list[0]
    el = e_list[1]
    for key in el_0:
        seq = el_0[key][0]
        for j in range(n - 1, 0, -1):
            p = seq[0:j] + [n - 1] + seq[j:]
            res = extendedSig(p, key, n)
            if not res:
                break
            s = str(res)
            c = el_0[key][1]
            if s in el:
                el[s][1] += c
            else:
                el[s] = [p, c]

    e_list[0] = el
    e_list[1] = {}
    s = 0
    for key in el:
        s += el[key][1]
    return s

for k in range(16):
    print([k, A(k)])
