c(n) = {
    my(sum = 0);
    for(i=0, n,
        for(j=0, n,
            for(k=0, n,
                if(i + j + k == n,
                    sum += abs(stirling(i+n, n, 2) * stirling(j+n, n, 2) * stirling(k+n, n, 2));
                )
            )
        )
    );
    sum;
};

for(n=0, 20, print1(c(n),", "));
