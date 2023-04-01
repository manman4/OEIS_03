a(n) = {
    v=[1, -9];
    if(n<3, 
        v[n],
        -n^3 * sumdiv(n, d, if(d<n, a(d) / d^3))
    )
}

for(n=1, 30, print1(a(n),", ")) 
