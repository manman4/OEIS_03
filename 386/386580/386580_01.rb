#!/usr/bin/env ruby
# frozen_string_literal: true

# A386580: Number of normal multisets of size n having a permutation
#          with all distinct run lengths.
#
# 方針
#   大きさ n の正規多重集合は、重複度の列 (m_1, ..., m_k) すなわち n の組成と
#   一対一に対応する。条件は重複度の多重集合（分割 λ）だけで決まるので、
#
#     a(n) = Σ_{λ ⊢ n, λ が条件を満たす} ℓ(λ)! / Π_j m_j(λ)!
#
#   として計算する（各分割を、その並べ替えである組成の個数で重み付け）。
#
# 分割 λ = (m_1, ..., m_k) が条件を満たす ⇔ 次を同時に満たす選び方がある:
#   (1) 各 m_i を相異なる部分に分割 π_i し、全 π_i の部分が互いに重ならない
#   (2) r_i = ℓ(π_i) として 2 * max r_i <= Σ r_i + 1
#       （同じ値のランが隣接しないように並べられる条件）
#
# 使い方
#   ruby 386580_01.rb [N]          # a(0..N) を表示（既定 N = 20）
#   ruby 386580_01.rb [N] --check  # n <= 8 で総当たりと照合してから表示

# n の分割を降順の配列で列挙する
def partitions(n, max = n, &block)
  return enum_for(:partitions, n, max) unless block
  if n.zero?
    yield []
    return
  end
  [n, max].min.downto(1) do |p|
    partitions(n - p, p) { |rest| yield [p] + rest }
  end
end

# m を相異なる部分に分割し、使用済み部分（ビットマスク used）と重ならないものについて
# [その分割の部分集合のマスク, 部分の個数] を yield する
def strict_splits(m, max, used, mask = 0, len = 0, &block)
  if m.zero?
    yield mask, len
    return
  end
  [m, max].min.downto(1) do |p|
    bit = 1 << p
    next if used & bit != 0
    strict_splits(m - p, p - 1, used | bit, mask | bit, len + 1, &block)
  end
end

# 分割 mults が条件 (1)(2) を満たすかどうか
def good?(mults, memo = {})
  key = mults.sort.reverse
  return memo[key] if memo.key?(key)

  # 重複度がすべて異なれば、各値を 1 つのランにまとめればよい
  return memo[key] = true if key.uniq.size == key.size

  found = false
  search = lambda do |i, used, r_sum, r_max|
    return if found
    if i == key.size
      found = true if 2 * r_max <= r_sum + 1
      return
    end
    strict_splits(key[i], key[i], used) do |mask, len|
      search.call(i + 1, used | mask, r_sum + len, [r_max, len].max)
      break if found
    end
  end
  search.call(0, 0, 0, 0)
  memo[key] = found
end

def factorial(k)
  (1..k).reduce(1, :*)
end

# 分割 λ の並べ替え（組成）の個数 ℓ(λ)! / Π m_j(λ)!
def compositions_count(lam)
  lam.tally.values.reduce(factorial(lam.size)) { |acc, c| acc / factorial(c) }
end

def a386580(n, memo = {})
  partitions(n).sum { |lam| good?(lam, memo) ? compositions_count(lam) : 0 }
end

# ---- 検証用：定義どおりの総当たり ----

# 多重集合の相異なる並べ替えを列挙する
def multiset_permutations(counts, acc = [], &block)
  if counts.all?(&:zero?)
    yield acc
    return
  end
  counts.each_index do |v|
    next if counts[v].zero?
    counts[v] -= 1
    acc.push(v)
    multiset_permutations(counts, acc, &block)
    acc.pop
    counts[v] += 1
  end
end

def distinct_run_lengths?(seq)
  lens = seq.chunk_while { |x, y| x == y }.map(&:size)
  lens.uniq.size == lens.size
end

# 正規多重集合（= 組成）を直接すべて調べる
def a386580_brute(n)
  return 1 if n.zero?
  count = 0
  (1 << (n - 1)).times do |bits|
    comp = []
    cur = 1
    (n - 1).times do |i|
      if bits[i] == 1
        comp << cur
        cur = 1
      else
        cur += 1
      end
    end
    comp << cur
    ok = false
    multiset_permutations(comp.dup) do |perm|
      if distinct_run_lengths?(perm)
        ok = true
        break
      end
    end
    count += 1 if ok
  end
  count
end

# ---- メイン ----

if __FILE__ == $PROGRAM_NAME
  n_max = (ARGV.find { |s| s =~ /\A\d+\z/ } || 20).to_i

  if ARGV.include?('--check')
    memo = {}
    0.upto([n_max, 8].min) do |n|
      fast = a386580(n, memo)
      slow = a386580_brute(n)
      raise "mismatch at n=#{n}: #{fast} vs #{slow}" unless fast == slow
    end
    puts "check ok (n <= #{[n_max, 8].min})"
  end

  memo = {}
  terms = (0..n_max).map { |n| a386580(n, memo) }
  terms.each_with_index { |v, n| puts "#{n} #{v}" }
  puts terms.join(', ')
end
