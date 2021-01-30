def A(n)
  cnt = 0
  (1..n).to_a.permutation{|a|
    if (1..n).all?{|i| a[i - 1] % i == 0 || i % a[i - 1] == 0}
      cnt += 1
    end
  }
  cnt
end

(1..12).each{|i| p A(i)}
