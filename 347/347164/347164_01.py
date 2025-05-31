def has_all_digits(s):
    return set(s) == set("0123456789")

def find_exponents_with_all_digits_mod(m):
    found = []
    n = 1
    mod_base = 10**10
    while len(found) < m:
        last_digits_int = pow(2, n, mod_base)
        last_digits_str = str(last_digits_int).rjust(10, '0')  # ゼロ埋めで10桁に
        if has_all_digits(last_digits_str):
            found.append((n, last_digits_str))
        n += 1
    return found

# 使用例：10個見つける
M = 10
results = find_exponents_with_all_digits_mod(M)
for i, (n, digits) in enumerate(results, 1):
    print(f"{i:2d}: n = {n}, 下10桁 = {digits}")

