def carryless(f_ary, b_ary)
  s1, s2 = f_ary.size, b_ary.size
  ary = Array.new(s1 + s2 - 1, 0)
  (0..s1 - 1).each{|i|
    (0..s2 - 1).each{|j|
      ary[i + j] += f_ary[i] * b_ary[j]
    }
  }
  ary.map{|i| i % 10}.join.to_i
end

def T(n, k)
  carryless(n.to_s.split('').map(&:to_i), k.to_s.split('').map(&:to_i))
end

n = 99
cnt = 1
(1..n).each{|i|
  (1..i).each{|j|
    print cnt
    print ' '
    puts T(i, j)
    cnt += 1
  }
}
