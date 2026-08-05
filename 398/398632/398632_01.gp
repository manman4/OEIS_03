\\ a(n) = Sum_{k=0..4} (-2)^k * binomial(4,k) * (n+4-k)! for n >= 4.
print("境界がだめ");
a(n) = my(v=[24, 14, 152, 1392]); if(n<3, v[n+1], sum(k=0, 4, (-2)^k*binomial(4, k)*(n+4-k)!));
for(n=0, 19, print1(a(n),", "));

a(n) = my(v=[24, 14, 152, 1392]); if(n<4, v[n+1], sum(k=0, 4, (-2)^k*binomial(4, k)*(n+4-k)!));
for(n=0, 19, print1(a(n),", "));
