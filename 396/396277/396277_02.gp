\\ Example 
N=50;
B(v) = v * (1 + v)^3; 
A0 = x + x*O(x^N);
A1 = B(x + x*O(x^N));
A2 = B(B(x + x*O(x^N)));
A3 = B(B(B(x + x*O(x^N))));

print(A0);
print(A1);
print(A2);
print(A3);

Vec((A0/x)^(1/3))
Vec((A1/x)^(1/3))
Vec((A2/x)^(1/3))
Vec((A3/x)^(1/3))