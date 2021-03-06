# 和因子はmin以上max以下で互いに異なる
def partition(n, min, max)
  return [[]] if n == 0
  [max, n].min.downto(min).flat_map{|i| partition(n - i, min, i - 1).map{|rest| [i, *rest]}}
end

n = 100
ary = []
(1..n).each{|i|
  cnt = 0
  s_ary = Array.new(i, 0)
  partition(i, 1, i).each{|a|
    a = a.reverse
    flag = true
    s = a[0]
    (2..a.size).each{|i|
      s0 = (a[i - 1] / i.to_r)
      if s0 >= s
        s = s0
      else
        flag = false
      end
    }
    if flag
      s_ary[a[0] - 1] += 1
    end
  }
  ary << s_ary
}
a = ary.flatten
(1..a.size).each{|i|
  print i
  print ' '
  puts a[i - 1]
}
