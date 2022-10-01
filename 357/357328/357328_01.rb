def A(n)
  cnt = 0
  (2..n).to_a.permutation{|i|
    perm = [1] + i
    flag = true
    (1..n).each{|j|
      m = perm[j - 1]
      (1..j).each{|k|
        if j % k == 0
          flag = false if m % perm[k - 1] > 0
        end
      }
    }
    cnt += 1 if flag
  }
  cnt
end

p (1..13).map{|i| p A(i)}

# かなり時間がかかる
# [1, 1, 2, 1, 2, 1, 2, 2, 2, 1, 2, 2, 6]
