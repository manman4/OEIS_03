def is_square(n)
  m = Math.sqrt(n).to_i
  m * m == n
end

# Number of solutions to a^2 + n^2 = b^2 + c^2 with 1 <= a < b < c < n
def A(n)
  cnt = 0
  (1..n).each{|a|
    (a + 1..n).each{|b|
        c2 = a * a + n * n - b * b
        cnt += 1 if c2 > b * b && is_square(c2)
    }
  }
  cnt
end

n = 1000
s = 0
(1..n).each{|i|
  s += A(i)
  print i
  print ' '
  puts s
}
