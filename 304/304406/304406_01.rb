# ˜aˆöq‚ÍminˆÈãmaxˆÈ‰º
def partition(n, min, max)
  return [[]] if n == 0
  [max, n].min.downto(min).flat_map{|i| partition(n - i, min, i).map{|rest| [i, *rest]}}
end

s_ary = []
(1..15).each{|i|
  cnt = 0
  partition(i, 1, i).each{|ary|
    a = ary.group_by{|i| i}.sort
    m = a[0][1].inject(:+)
    flag = true
p_ary = []
    a.each{|b|
      mb = b[1].inject(:+)
      if m >= mb
        m = mb
      else
        flag = false
      end
p_ary << m
    }
    if flag
p [a, p_ary]
      cnt += 1
    end
  }
  s_ary << cnt
}
p s_ary
