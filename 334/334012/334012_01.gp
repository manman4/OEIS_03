a(n) = n*(3*n-2);
S(k, m) = sum(j=k, k+m-1, a(j));
print(S(1, 1));
print(S(19, 1));
print(S(1, 10));
print(S(45, 1));
print(S(11, 9));
print(S(1, 18));