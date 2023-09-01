# HangYan Readme

## 分支说明

master: 发布分支  
develop-xx: 研发本地自己的开发分支, 测试通过后可合并至master  
 
建议通过git-rebase的方式进行合并,流程如下： 

eg: 
当前本地开发分支为：develop-xx (在当前develop-xx暂存修复记录并提交到本地)  

1. 确保基于最新代码开始：  
git checkout master     // 切换到主分支  
git pull origin master  // 拉取最新的主分支代码  

2. 切换到开发分支：
git checkout develop-xx  // 切换到开发分支  

3. 执行rebase操作: 这将会将开发分支的提交逐个应用到主分支的顶部，同时保持提交历史的线性性。  
git rebase master  // 在开发分支上执行rebase操作  

4. 解决冲突（如果有）   
git add .  // 添加解决冲突后的更改  
git rebase --continue  // 继续rebase操作  

5. 完成rebase操作  

6. 切换回主分支  
git checkout master  // 切换回主分支  

7. 合并开发分支  
git merge develop-xx  // 将开发分支的更改合并到主分支  

8. 推送更改到远程仓库  
git push origin master  // 推送主分支的更改到远程仓库  