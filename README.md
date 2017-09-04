# Fuji_log_parser

功能簡介: (1)collector:於程式運作期間即時備存每次對位使用的Mark_ID.tif檔，並依生產日期/產品名/層別/批號/流水號歸檔。(2)classifier:將Mark_ID.tif檔依黑白靶/外型/照明類型/尺寸做粗分類，並計算相同類別內靶點間相似度。(3)score_matrix:將相同類別內Mark_ID.tif檔進一步依彼此間相似度做細分類，並將同類別歸檔。以下為詳細說明。

## 1.Fuji曝光機 product log產生流程
### 1-1.Product log 製作流程圖
![picture alt](https://github.com/bn90207/Fuji_log_parser/blob/master/illustrations/exposurer_flowchart.jpg?raw=true) 

Fuji曝光機將所有對位參數儲存於Mark_ID.tif檔內，人員可於生產期間依需求任意修改，但設備未紀錄其修改履歷。由上方流程圖可知每次設備生產時均會產生一份AF.log檔，其檔名包含生產時間。故可藉由不斷檢查有無新的AF.log檔產生，來確認是否有對位事件發生，並啟動程式備存產品資料與Mark_ID.tif。

## 2.Collector
### 2-1.Collector 執行流程圖
![picture alt](https://github.com/bn90207/Fuji_log_parser/blob/master/illustrations/collector_flowchart.png?raw=true) 


如上節所述，Collector於執行期間會不斷搜尋設備端logshare內，是否有較上次紀錄時間更晚產生的AF.log檔。當新的AF.log檔產生時，再依序讀取job info.xml、jobs.csv、Mark_ID.tif取得生產時產品資料與對位參數。

### 2-2.Collector 輸出形式
![picture alt](https://github.com/bn90207/Fuji_log_parser/blob/master/illustrations/collector.png?raw=true)

每次程式取得產品資料與對位參數會將其輸出至指定路徑(up_output_path/dn_output_path)
