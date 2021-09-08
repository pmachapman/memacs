# MicroEMACS
This is an unofficial repository for the MicroEMACS Editor by Daniel Lawrence.

## Introduction
I have found it hard to track down the source code for MicroEMACS, 
and with Daniel's passing I have noticed that links from his home page are breaking.

This project seeks to collect the source code and binaries for MicroEMACS, and provide them all from one place

## Readme

Please refer to [DOC/README](doc/readme) for the complete MicroEMACS readme file.

## Copyright

This code retains the copyright Daniel placed on this work (refer to [LICENSE.md](LICENSE.md)), and on his passing I assume it is now with his estate.

I will endeavour to respect this, and request that you do the same, until notified otherwise.


http://dfrench.hypermart.net/js/Downloads/Editors/MicroEMACS/index.content.shtml


```
(prefer-coding-system 'utf-8)
(set-language-environment "Chinese-GB")
(set-locale-environment "Chinese-GB")
(set-default-coding-systems 'utf-8)
(set-terminal-coding-system 'utf-8)
(set-keyboard-coding-system 'utf-8)

(when (eq system-type 'windows-nt)
 (setq fonts '("Consolas NF" "Microsoft YaHei-X"))
 (set-face-attribute 'default nil :font
                     (format "%s:pixelsize=%d" (car fonts) 16))
 ;; (setq face-font-rescale-alist '(("微软雅黑". 1.1)))
 )

(dolist (charset '(kana han symbol cjk-misc bopomofo))
 (set-fontset-font (frame-parameter nil 'font) charset
                   (font-spec :family (car (cdr fonts)))))

(setq default-frame-alist
'((height . 32) (width . 100) (menu-bar-lines . 20) (tool-bar-lines . 0)))

(setq package-gnupghome-dir "/c/Users/polaris/AppData/Roaming/.emacs.d/elpa/gnupg")
(require 'package)
(package-initialize)
(add-to-list 'package-archives '("melpa" . "https://melpa.org/packages/") t)
(load-theme 'monokai t)
```
