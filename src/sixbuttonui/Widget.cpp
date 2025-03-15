#include "Widget.h"

void Widget::preLoadModel() {
  WidgetModel* model = getModel();
  if (!model) return;

  model->setTitleRaw(_wConf->getTitle(), _wConf->isTitlePmem());
  model->setInstructionRaw(_wConf->getInstruction(), _wConf->isInstructionPmem());
  model->setFooterRaw(_wConf->getFooter(), _wConf->isFooterPmem());
}

void Widget::populateModel(void* state) {
  if (!_modelLoaded) {
    initModel();
    preLoadModel();
  }
  if (!_modelLoaded || !_noRefreshModel) {
    loadModel(state);
  }
  _modelLoaded = true;
}
