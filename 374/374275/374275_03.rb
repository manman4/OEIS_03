#!/usr/bin/env ruby
# frozen_string_literal: true
#
# A374275
#   Smallest k such that k can be written in exactly n ways as
#   x^2 + 3*x*y + y^2 with 0 <= x <= y.
#
# ------------------------------------------------------------------
# 理論（判別式 +5。不定符号形式、実二次体 Q(sqrt 5)）
# ------------------------------------------------------------------
#   x^2 + 3xy + y^2 の判別式は 3^2 - 4 = 5 で正。判別式 5 は基本判別式で
#   Z[(1+sqrt5)/2] は最大整環、類数は 1。単数群は無限だが、x, y >= 0 に
#   限れば x^2+3xy+y^2 >= max(x,y)^2 なので解は有限個で、全正値単数の作用に
#   よる各軌道からちょうど 1 点が半開基本領域に入る。
#
#   k = 5^t * Π p_i^{e_i} * Π q_j^{f_j}
#       p_i ≡ 1, 4 (mod 5) : 分解  11, 19, 29, 31, 41, 59, 61, 71, ...
#       q_j ≡ 2, 3 (mod 5) : 惰性（2, 3, 7, 13, 17, 23, ...）
#       5 : 分岐
#
#   M(k) := Π(e_i + 1)   ... f_j が全て偶数のとき
#         := 0           ... それ以外
#   （5 の指数 t は M に効かない）
#
#   姉妹列の数え方と比べると次のようになる。
#       A374091: x, y > 0 の順序付き    P(k) = M(k) - [k は平方数]
#       A374095: 0 < x < y             U(k) = floor(M(k) / 2)
#       本数列 : 0 <= x <= y           V(k) = U(k) + eps(k)
#   ここで追加される退化解は x=0（k が平方数、y=sqrt k）と x=y（5x^2=k、
#   すなわち k が 5 倍の平方数）で、
#       eps(k) := [k は平方数] + [k は 5 倍の平方数] ∈ {0, 1}
#   （k = u^2 = 5v^2 は 5 進付値の偶奇が合わないので両立しない）。
#
#   さらに eps は M のパリティで自動的に決まる:
#       M が奇 <=> 全 e_i が偶 <=> k = 5^t * (平方数)
#                 => t 偶なら平方数、t 奇なら 5 倍の平方数 => eps = 1
#       M が偶 <=> ある e_i が奇 => k は平方数でも 5 倍平方数でもない => eps = 0
#   よって
#       V(k) = floor(M/2) + (M mod 2) = ceil( M(k) / 2 )
#   （k <= 200000 で総当たりと完全一致）
#
# ------------------------------------------------------------------
# 最小化
# ------------------------------------------------------------------
#   V(k) = n (n >= 2) となるのは M(k) ∈ {2n-1, 2n} のときだけ。5^t や
#   q^(偶数) は M を変えず k を大きくするだけなので、分解素数だけを使えばよい。
#   したがって候補はわずか 2 個:
#
#       a(n) = min( minprod(2n-1), minprod(2n) )    (n >= 2)
#
#   minprod(M) := 5k±1 型素数 11, 19, 29, 31, ... のみを使って
#                 Π(e_i + 1) = M となる最小の数
#
#   例外:
#     a(0) = 2  ... M(k) = 0 となる最小の k。2 は 2 (mod 5) の惰性素数で
#                   指数 1 なので表現不可能。
#     a(1) = 0  ... k = 0 は (0, 0) の 1 解を持ち、これが最小。
#                   （公式側は minprod(1) = 1 を返す。k = 1 も (0, 1) の
#                     1 解を持つが、0 のほうが小さい）
#
#   姉妹列との関係:
#     A374095（0 < x < y）は M ∈ {2n, 2n+1} を見るのに対し、こちらは
#     M ∈ {2n-1, 2n}。退化解 2 個を足したぶん添字が 1 つずれる。
#
# ------------------------------------------------------------------
# 系
#   11 | a(n)（n >= 1）。n >= 2 では 2n-1 >= 3 かつ 2n >= 4 なので、
#   どちらの候補も M >= 2 で最小の分解素数 11 を必ず含む。a(1) = 0 も
#   11 の倍数。a(0) = 2 だけが例外。
# ------------------------------------------------------------------
#
# 使い方:
#   ruby 374275_03.rb           # a(n) が 1000 桁を超える手前まで出力
#   ruby 374275_03.rb 30        # n = 0..30 を出力（桁数制限も併用）
#   ruby 374275_03.rb --digits=200   # 桁数の上限を変更
#   ruby 374275_03.rb --known   # 総当たりで確定済みの項と照合
#   ruby 374275_03.rb --verify  # 総当たりと突き合わせて検証

module A374275
  module_function

  # 出力する a(n) の桁数上限。ちょうどこの桁数までは出力し、
  # 超えた時点で打ち切る（1000 桁は OK、1001 桁で停止）。
  MAX_DIGITS = 1000

  # ---------- 素数まわり（prime gem に依存しない） ----------

  def prime?(n)
    return false if n < 2
    return true  if n < 4
    return false if n.even?

    d = 3
    while d * d <= n
      return false if (n % d).zero?
      d += 2
    end
    true
  end

  # p ≡ 1, 4 (mod 5) な素数（5 で分解する素数）を小さい順に count 個
  def primes_split5(count)
    ps = []
    n = 11
    while ps.size < count
      ps << n if [1, 4].include?(n % 5) && prime?(n)
      n += 1
    end
    ps
  end

  # ---------- minprod ----------

  # Π(e_i + 1) = m となる最小の Π primes[i]^{e_i}
  # 指数は非増加としてよい（小さい素数に大きい指数を割り当てるのが最適）
  def minprod(m, primes = nil, idx = 0, max_exp = Float::INFINITY, memo = {})
    return 1 if m == 1

    primes ||= primes_split5(Math.log2(m).floor + 1)
    key = [m, idx, max_exp]
    cached = memo[key]
    return cached if cached
    return nil if idx >= primes.size

    best = nil
    divisors_from(m, 2).each { |d|
      e = d - 1
      next if e > max_exp

      rest = minprod(m / d, primes, idx + 1, e, memo)
      next if rest.nil?

      cand = primes[idx]**e * rest
      best = cand if best.nil? || cand < best
    }
    memo[key] = best
  end

  def divisors_from(m, lower)
    ds = []
    d = 1
    while d * d <= m
      if (m % d).zero?
        ds << d if d >= lower
        e = m / d
        ds << e if e != d && e >= lower
      end
      d += 1
    end
    ds.sort
  end

  # ---------- 本体 ----------

  def a(n)
    return 2 if n.zero? # M(k) = 0 となる最小の k
    return 0 if n == 1  # k = 0 -> (0, 0)

    cands = []
    [2 * n - 1, 2 * n].each { |m|
      v = minprod(m)
      cands << v if v
    }
    cands.min
  end

  # a(n) の桁数が max_digits を超えた時点で打ち切る。
  # nmax を与えた場合はそこでも打ち切る（どちらか早いほう）。
  # 戻り値は [n, a(n)] の配列。
  def sequence(nmax = nil, max_digits = MAX_DIGITS)
    out = []
    n = 0
    loop {
      break if nmax && n > nmax

      v = a(n)
      break if v.to_s.size > max_digits

      out << [n, v]
      n += 1
    }
    out
  end

  # k <= 6*10^7 の総当たり走査で確定した項
  KNOWN = {
    0 => 2, 1 => 0, 2 => 121, 3 => 2299, 4 => 6061, 5 => 43681,
    6 => 66671, 7 => 33659659, 8 => 187891, 9 => 1266749, 10 => 8067191,
    12 => 2066801, 14 => 36735721, 16 => 7703531, 18 => 39269219
  }.freeze

  def check_known
    ok = true
    KNOWN.keys.sort.each { |n|
      want = KNOWN[n]
      got = a(n)
      good = (got == want)
      ok &&= good
      puts format('%-4s n=%-3d got=%-14d known=%d', good ? 'OK' : 'NG', n, got, want)
    }
    puts ok ? "\nmatches all #{KNOWN.size} known terms." : "\nMISMATCH."
    ok
  end

  # ---------- 検証用の総当たり ----------

  # x^2 + 3*x*y + y^2 = k を満たす 0 <= x <= y の個数
  # y^2 + 3x*y + (x^2 - k) = 0 を y について解く（判別式 5x^2 + 4k）
  def count_pairs(k)
    return 1 if k.zero?

    cnt = 0
    x = 0
    while 5 * x * x <= k
      disc = 5 * x * x + 4 * k
      s = Integer.sqrt(disc)
      if s * s == disc
        num = -3 * x + s
        cnt += 1 if !num.negative? && num.even? && num / 2 >= x
      end
      x += 1
    end
    cnt
  end

  def verify(limit = 5_000_000, nmax = 40)
    warn "brute force scan up to k = #{limit} ..."
    cnt = Array.new(limit + 1, 0)
    x = 0
    while 5 * x * x <= limit
      y = x
      loop {
        k = x * x + 3 * x * y + y * y
        break if k > limit

        cnt[k] += 1
        y += 1
      }
      x += 1
    end

    first = {}
    cnt.each_with_index { |c, k| first[c] ||= k }

    ok = true
    (0..nmax).each { |n|
      theory = a(n)
      brute  = first[n]

      if theory <= limit
        good = (brute == theory)
        puts format('%-4s n=%-4d theory=%-16d brute=%s',
                    good ? 'OK' : 'NG', n, theory, brute.inspect)
      else
        good = brute.nil?
        puts format('%-4s n=%-4d theory=%-16d (> limit, brute=%s)',
                    good ? 'skip' : 'NG', n, theory, brute.inspect)
      end
      ok &&= good
    }
    puts ok ? "\nall matched." : "\nMISMATCH FOUND."
    ok
  end
end

if __FILE__ == $PROGRAM_NAME
  if ARGV.include?('--known')
    A374275.check_known
  elsif ARGV.include?('--verify')
    nums = ARGV.select { |s| s =~ /\A\d+\z/ }.map(&:to_i)
    A374275.verify(nums[0] || 5_000_000, nums[1] || 40)
  else
    dig = ARGV.find { |t| t =~ /\A--digits=(\d+)\z/ } ? Regexp.last_match(1).to_i
                                                      : A374275::MAX_DIGITS
    cap = ARGV.find { |t| t =~ /\A\d+\z/ }&.to_i
    rows = A374275.sequence(cap, dig)
    rows.each { |n, v| puts "#{n} #{v}" }
    last = rows.last
    if last && (cap.nil? || last[0] < cap)
      nxt = last[0] + 1
      warn "stopped: a(#{nxt}) has #{A374275.a(nxt).to_s.size} digits (> #{dig})"
    end
  end
end
