def d(n)
  sum = 0
  (1..n).each{|i|
    sum += i if i % 2 == 1 and n % i == 0
  }
  sum
end

def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

def A(n)
  return 0 if n < 3
  s = 0
  (1..n - 2).each{|i|
    (1..n - 2).each{|j|
      (1..n - 2).each{|k|
        s += d(i) * d(j) * d(k) / (i * j * k).to_r if i + j + k == n
      }
    }
  }
  (f(n) * s).to_i / 6
end

n = 1000
(1..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}

# def B(n)
#   s = 0
#   (1..n - 3).each{|i|
#     (1..n - 3).each{|j|
#       (1..n - 3).each{|k|
#         (1..n - 3).each{|l|
#           s += d(i) * d(j) * d(k) * d(l) / (i * j * k * l).to_r if i + j + k + l == n
#         }
#       }
#     }
#   }
#   (f(n) * s).to_i / 24
# end

# (4..n).each{|i|
#   j = B(i)
#   break if j.to_s.size > 1000
#   print i
#   print ' '
#   puts j
# }