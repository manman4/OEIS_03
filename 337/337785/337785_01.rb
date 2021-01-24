def f(n)
  ary = [1]
  (n - 1).times{|i|
    ary = [0] + ary + [0]
    ary = (0..i + 1).map{|j| ary[j] + ary[j + 1] + 1}
  }
  ary
end

def A(n)
  f_ary = (1..n / 2).map{|i| [i]}
  cnt = 1
  s = 1
  while f_ary.size > 0
    s_ary = f(s + 1)
    b_ary = []
    f_ary.each{|i|
      (1..i[0] - 1).each{|j|
        a = [j]
        (0..s - 1).each{|k|
          num = i[k] - a[k]
          if num > 0
            a << num
          else
            break
          end
        }
        if a.size == s + 1
          sum = (0..s).inject(0){|t, m| t + s_ary[m] * a[m]}
          if sum < n
            b_ary << a
          elsif sum == n
            cnt += 1
          end
        end
      }
    }
    f_ary = b_ary
    s += 1
  end
  cnt
end

n = 500
(499..n).each{|i|
  j = A(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
